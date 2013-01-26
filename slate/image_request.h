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

#ifndef __IMAGE_REQUEST__
#define __IMAGE_REQUEST__

#include <QObject>
#include <QByteArray>
#include <QHash>
#include <QTimer>

#include "image_id.h"
#include "dnd_server.h"

class DnDClient;
class ImageTransfer;

class ImageRequest : public QObject
{
  Q_OBJECT

public: 
  ImageRequest (const ImageId& image_id, QObject* parent = nullptr);

  void add_query_client (DnDServer::ClientRecord* client);
  void add_pending_client (DnDServer::ClientRecord* client);
  void execute ();

signals:
  void complete (const ImageId& image_id, const QByteArray& data);

private slots:
  void image_query (DnDClient* client, const ImageId& image_id);
  void query_timeout ();
  void transfer_complete (const QByteArray& data);

private:
  ImageId _image_id;
  QHash<DnDClient*, DnDServer::ClientRecord*> _query_clients;
  QHash<DnDClient*, DnDServer::ClientRecord*> _pending_clients;
  DnDServer::ClientRecord* _best_client;
  bool _started;
  QTimer _query_timeout;
  ImageTransfer* _transfer;

  void begin_transfer (DnDServer::ClientRecord* client);
};

#endif /* __IMAGE_REQUEST__ */
