/* Copyright (c) 2013, Dustin Mitchell dmmitche <at> gmail <dot> com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the document
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "image_request.h"
#include "dnd_client.h"
#include "util.h"
#include "image_transfer.h"

ImageRequest::ImageRequest (const ImageId& image_id, QObject* parent)
  : QObject (parent), _image_id (image_id), _best_client (nullptr),
    _started (false)
{
}

void ImageRequest::add_query_client (DnDServer::ClientRecord* client)
{
  if (_started || _query_clients.count (client->client) != 0)
    return;

  connect (client->client, SIGNAL (image_query (DnDClient*, const ImageId&)),
    SLOT (image_query (DnDClient*, const ImageId&)));

  _query_clients.insert (client->client, client);
}

void ImageRequest::add_pending_client (DnDServer::ClientRecord* client)
{
  if (_pending_clients.count (client->client) != 0)
    return;

  _pending_clients.insert (client->client, client);
}

void ImageRequest::execute ()
{
  if (!_started) {
    _started = true;

    for_all (_query_clients, [] (DnDServer::ClientRecord* r, const ImageId& id)
                             {
                               r->client->image_query (id);
                             }, _image_id);
  }
}

void ImageRequest::image_query (DnDClient* client, const ImageId&)
{
  DnDServer::ClientRecord* record = _query_clients[client];

  if (record->sending_image && !_best_client) {
    _best_client = record;
    _query_timeout.setSingleShot (true);
    _query_timeout.start (500);
  }
  else
    begin_transfer (record);
}

void ImageRequest::query_timeout ()
{
  if (_best_client)
    begin_transfer (_best_client);
}

void ImageRequest::transfer_complete (const QByteArray& data)
{
  for_all (_pending_clients,
    [] (DnDServer::ClientRecord* r, const QByteArray& d)
    {
      r->client->send_image (d);
    }, data);

  complete (_image_id, data);
}

void ImageRequest::begin_transfer (DnDServer::ClientRecord* client)
{
  _query_timeout.stop ();
  _best_client = nullptr;

  for_all (_query_clients, [&] (DnDServer::ClientRecord* r)
                           {
                             disconnect (r->client);
                           });

  _transfer = new ImageTransfer (client->client);

  connect (_transfer, SIGNAL (complete (const QByteArray&)),
    SLOT (transfer_complete (const QByteArray&)));

  client->sending_image = true;
  client->client->request_image (_image_id);
}
