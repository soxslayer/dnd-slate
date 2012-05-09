/* Copyright (c) 2012, Dustin Mitchell dmmitche <at> gmail <dot> com
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

#include <QHostAddress>
#include <QFile>

#include "dnd_server.h"
#include "dnd_client.h"
#include "dnd_messages.h"
#include "tile.h"

DnDServer::DnDServer (quint16 port)
{
  listen (QHostAddress::Any, port);
}

DnDServer::~DnDServer ()
{
  QMap<Uuid, ClientId*>::iterator beg = _client_map.begin ();
  QMap<Uuid, ClientId*>::iterator end = _client_map.end ();

  for (; beg != end; ++beg)
    delete *beg;
}

void DnDServer::incomingConnection (int socketDescriptor)
{
  DnDClient* client = new DnDClient (this);
  client->setSocketDescriptor (socketDescriptor);

  connect (client, SIGNAL (comm_proto_req (DnDClient*)),
           this, SLOT (client_comm_proto_req (DnDClient*)));
  connect (client, SIGNAL (user_add_req (DnDClient*, const QString&)),
           this, SLOT (client_user_add_req (DnDClient*, const QString&)));
  connect (client,
    SIGNAL (chat_message (DnDClient*, Uuid, Uuid, const QString&, int)), this,
    SLOT (client_chat_message (DnDClient*, Uuid, Uuid, const QString&, int)));
  connect (client, SIGNAL (load_image (DnDClient*, const QString&)),
           this, SLOT (client_load_image (DnDClient*, const QString&)));
  connect (client, SIGNAL (add_tile (DnDClient*, quint32, quint8, quint16,
                           quint16, quint16, quint16, const QString&)),
           this, SLOT (client_add_tile (DnDClient*, quint32, quint8, quint16,
                       quint16, quint16, quint16, const QString&)));
  connect (client, SIGNAL (disconnected (DnDClient*)),
           this, SLOT (client_disconnected (DnDClient*)));
}

void DnDServer::client_disconnected (DnDClient* client)
{
  QMap<Uuid, ClientId*>::iterator beg = _client_map.begin ();
  QMap<Uuid, ClientId*>::iterator end = _client_map.end ();
  Uuid rm_uuid = 0;
  QMap<Uuid, ClientId*>::iterator rm;

  for (; beg != end; ++beg) {
    if (client == (*beg)->client) {
      rm = beg;
      rm_uuid = beg.key ();
      break;
    }
  }

  if (beg == end) {
    qDebug () << "Not found";
    return;
  }

  _client_map.erase (rm);

  beg = _client_map.begin ();

  for (; beg != end; ++beg)
    (*beg)->client->user_del (rm_uuid);
}

void DnDServer::client_comm_proto_req (DnDClient* client)
{
  client->comm_proto_resp (COMM_PROTO_MAJOR, COMM_PROTO_MINOR);
}

void DnDServer::client_user_add_req (DnDClient* client, const QString& name)
{
  Uuid uuid = _uuid_manager.get_uuid ();
  QString c_name = name;

  if (_client_map.isEmpty ()) {
    _dm_client = client;
    _dm_uuid = uuid;
    QString dm_prefix = "[DM] ";
    c_name.prepend (dm_prefix);
  }

  c_name.replace (' ', '_');

  client->user_add_resp (uuid, c_name);
  send_map (client);

  QMap<Uuid, ClientId*>::iterator c_beg = _client_map.begin ();
  QMap<Uuid, ClientId*>::iterator c_end = _client_map.end ();

  for (; c_beg != c_end; ++c_beg) {
    (*c_beg)->client->user_add_resp (uuid, c_name);
    client->user_add_resp (c_beg.key (), (*c_beg)->name);
  }

  _client_map.insert (uuid, new ClientId (c_name, client));

  QList<Tile*>::iterator t_beg = _tiles.begin ();
  QList<Tile*>::iterator t_end = _tiles.end ();

  for (; t_beg != t_end; ++t_beg) {
    Tile* tile = *t_beg;
    client->add_tile (tile->get_uuid (), tile->get_type (), tile->x (),
                      tile->y (), tile->width (), tile->height (),
                      tile->get_text ());
  }
}

void DnDServer::client_chat_message (DnDClient* client, Uuid src, Uuid dst,
                                     const QString& message, int flags)
{
  /* Eliminates unused parameter warnings */
  (void)client;

  QMap<Uuid, ClientId*>::iterator beg = _client_map.begin ();
  QMap<Uuid, ClientId*>::iterator end = _client_map.end ();

  for (; beg != end; ++beg) {
    if (flags & CHAT_BROADCAST || src == beg.key () || dst == beg.key ())
      (*beg)->client->chat_message (src, dst, message, flags);
  }
}

void DnDServer::client_load_image (DnDClient* client, const QString& file_name)
{
  (void)client;

  if (client != _dm_client) {
    client->server_message ("Only the DM can load a map", MESSAGE_ERROR);
    return;
  }

  QFile image_file (file_name);
  if (!image_file.open (QIODevice::ReadOnly)) {
    client->server_message ("Cannot open file", MESSAGE_ERROR);
    return;
  }

  _map_data = image_file.readAll ();

  QMap<Uuid, ClientId*>::iterator beg = _client_map.begin ();
  QMap<Uuid, ClientId*>::iterator end = _client_map.end ();

  for (; beg != end; ++beg)
    send_map ((*beg)->client);
}

void DnDServer::client_add_tile (DnDClient* client, quint32 uuid, quint8 type,
                                 quint16 x, quint16 y, quint16 w, quint16 h,
                                 const QString& text)
{
  (void)client;

  Tile* tile = new Tile (_uuid_manager.get_uuid (), text,
                         static_cast<Tile::TileType> (type), x, y, w, h,
                         this);

  if (uuid != _dm_uuid)
    tile->add_perm (uuid, Tile::PERM_ALL);

  tile->add_perm (_dm_uuid, Tile::PERM_ALL);

  QMap<Uuid, ClientId*>::iterator beg = _client_map.begin ();
  QMap<Uuid, ClientId*>::iterator end = _client_map.end ();

  for (; beg != end; ++beg) {
    (*beg)->client->add_tile (tile->get_uuid (), tile->get_type (), tile->x (),
                              tile->y (), tile->width (), tile->height (),
                              tile->get_text ());
  }

  _tiles.append (tile);
}

void DnDServer::send_map (DnDClient* client)
{
  Uuid transfer_id = _uuid_manager.get_uuid ();

  if (!_map_data.size ())
    return;

  client->image_begin (_map_data.size (), transfer_id);

  int total_chunks = (_map_data.size () + DND_IMAGE_MAX_CHUNK_SIZE - 1)
                     / DND_IMAGE_MAX_CHUNK_SIZE;

  for (int i = 0; i < total_chunks; ++i) {
    int chunk_size = _map_data.size () - i * DND_IMAGE_MAX_CHUNK_SIZE;
    if (chunk_size > DND_IMAGE_MAX_CHUNK_SIZE)
      chunk_size = DND_IMAGE_MAX_CHUNK_SIZE;

    client->image_data (transfer_id, i,
      (uchar*)(_map_data.data () + i * DND_IMAGE_MAX_CHUNK_SIZE),
      chunk_size);
  }

  client->image_end (transfer_id);
}
