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
#include <QTimer>

#include "dnd_server.h"
#include "dnd_client.h"
#include "dnd_messages.h"
#include "tile.h"
#include "util.h"

#define PING_PONG_INTERVAL 1000

DnDServer::DnDServer (quint16 port)
{
  listen (QHostAddress::Any, port);

  QTimer* ping_pong_timer = new QTimer (this);

  connect (ping_pong_timer, SIGNAL (timeout ()),
           this, SLOT (ping_pong_timeout ()));

  ping_pong_timer->start (PING_PONG_INTERVAL);
}

DnDServer::~DnDServer ()
{
  for_all (_clients, [] (ClientRecord* r) { delete r->client; delete r; });
  for_all (_tiles, [] (Tile* t) { delete t; });
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
  connect (client, SIGNAL (add_tile (DnDClient*, Uuid, quint8, quint16,
                           quint16, quint16, quint16, const QString&)),
           this, SLOT (client_add_tile (DnDClient*, Uuid, quint8, quint16,
                       quint16, quint16, quint16, const QString&)));
  connect (client, SIGNAL (move_tile (DnDClient*, Uuid, quint16,
                           quint16)),
           this, SLOT (client_move_tile (DnDClient*, Uuid,
                       quint16, quint16)));
  connect (client, SIGNAL (delete_tile (DnDClient*, Uuid)),
           this, SLOT (client_delete_tile (DnDClient*, Uuid)));
  connect (client, SIGNAL (disconnected (DnDClient*)),
           this, SLOT (client_disconnected (DnDClient*)));
  connect (client, SIGNAL (ping_pong (DnDClient*)),
           this, SLOT (client_ping_pong (DnDClient*)));

  ClientRecord* rec = new ClientRecord;
  rec->client = client;

  if (_clients.isEmpty ())
    rec->player.set_dm ();

  _clients.insert (client, rec);
}

void DnDServer::client_disconnected (DnDClient* client)
{
  ClientRecord* dc_client = _clients[client];

  _clients.erase (_clients.find (client));

  for_all (_clients, [] (ClientRecord* r, Uuid u) { r->client->user_del (u); },
           dc_client->player.get_uuid ());
}

void DnDServer::client_comm_proto_req (DnDClient* client)
{
  client->comm_proto_resp (COMM_PROTO_MAJOR, COMM_PROTO_MINOR);
}

void DnDServer::client_user_add_req (DnDClient* client, const QString& name)
{
  ClientRecord* record = _clients[client];
  record->player.set_uuid (_uuid_manager.get_uuid ());
  record->active = true;

  QString t_name = name;

  if (record->player.get_dm ()) {
    t_name.prepend ("[DM]_");
    _dm = &record->player;
  }

  t_name.replace (' ', '_');
  record->player.set_name (t_name);

  record->client->user_add_resp (record->player.get_uuid (),
                                 record->player.get_name ());

  for_all (_clients,
           [&] (ClientRecord* r)
           {
             if (!r->active || r->client == record->client)
               return;

             r->client->user_add_resp (record->player.get_uuid (),
                                       record->player.get_name ());
             record->client->user_add_resp (r->player.get_uuid (),
                                            r->player.get_name ());
           });

  send_map (record->client);

  record->ping_pong.start ();
  record->client->ping_pong ();

  for_all (_tiles,
           [&] (Tile* t)
           {
             record->client->add_tile (t->get_uuid (), t->get_type (),
                                       t->get_x (), t->get_y (),
                                       t->get_width (), t->get_height (),
                                       t->get_text ());
           });
}

void DnDServer::client_chat_message (DnDClient* client, Uuid,
                                     Uuid dst_uuid, const QString& message,
                                     int flags)
{
  ClientRecord* record = _clients[client];

  for_all (_clients,
           [&] (ClientRecord* r)
           {
             if (!r->active)
               return;

             if (flags & CHAT_BROADCAST
                 || record->player.get_uuid () == r->player.get_uuid ()
                 || dst_uuid == r->player.get_uuid ())
               r->client->chat_message (record->player.get_uuid (), dst_uuid,
                                        message, flags);
           });
}

void DnDServer::client_load_image (DnDClient* client, const QString& file_name)
{
  ClientRecord* record = _clients[client];

  if (!record->player.get_dm ()) {
    record->client->server_message ("Only the DM can load a map",
                                    MESSAGE_ERROR);
    return;
  }

  QFile image_file (file_name);
  if (!image_file.open (QIODevice::ReadOnly)) {
    record->client->server_message ("Cannot open file", MESSAGE_ERROR);
    return;
  }

  _map_data = image_file.readAll ();

  for_all (_clients,
           [&] (ClientRecord* r)
           {
             if (!r->active)
               return;

             send_map (r->client);
           });
}

void DnDServer::client_add_tile (DnDClient* client, Uuid, quint8 type,
                                 quint16 x, quint16 y, quint16 w, quint16 h,
                                 const QString& text)
{
  ClientRecord* record = _clients[client];

  Tile* tile = new Tile (_uuid_manager.get_uuid (), text,
                         static_cast<Tile::TileType> (type), x, y, w, h);

  if (!record->player.get_dm ())
    tile->add_perm (record->player.get_uuid (), Tile::PERM_ALL);

  tile->add_perm (_dm->get_uuid (), Tile::PERM_ALL);

  for_all (_clients,
           [&] (ClientRecord* r)
           {
             if (!r->active)
               return;

             r->client->add_tile (tile->get_uuid (), tile->get_type (),
                                  tile->get_x (), tile->get_y (),
                                  tile->get_width (), tile->get_height (),
                                  tile->get_text ());
           });

  _tiles.insert (tile->get_uuid (), tile);
}

void DnDServer::client_move_tile (DnDClient* client, Uuid tile_uuid, quint16 x,
                                  quint16 y)
{
  ClientRecord* record = _clients[client];
  TileMap::iterator tile_iter = _tiles.find (tile_uuid);

  if (tile_iter == _tiles.end ()) {
    client->server_message ("Invalid tile UUID", MESSAGE_ERROR);
    return;
  }

  Tile* tile = *tile_iter;

  if (!tile->get_perm (record->player.get_uuid (), Tile::PERM_MOVE)) {
    client->server_message ("Move permission not granted", MESSAGE_ERROR);
    return;
  }

  tile->move (x, y);

  for_all (_clients,
           [&] (ClientRecord* r)
           {
             if (!r->active)
               return;

             r->client->move_tile (tile->get_uuid (), tile->get_x (),
                                   tile->get_y ());
           });
}

void DnDServer::client_delete_tile (DnDClient* client, Uuid tile_uuid)
{
  ClientRecord* record = _clients[client];
  TileMap::iterator tile_iter = _tiles.find (tile_uuid);

  if (tile_iter == _tiles.end ()) {
    client->server_message ("Invalid tile UUID", MESSAGE_ERROR);
    return;
  }

  Tile* tile = *tile_iter;

  if (!tile->get_perm (record->player.get_uuid (), Tile::PERM_MOVE)) {
    client->server_message ("Move permission not granted", MESSAGE_ERROR);
    return;
  }

  for_all (_clients,
           [&] (ClientRecord* r)
           {
             if (!r->active)
               return;

             r->client->delete_tile (tile_uuid);
           });

  _tiles.remove (tile_uuid);

  delete tile;
}

void DnDServer::client_ping_pong (DnDClient* client)
{
  ClientRecord* record = _clients[client];
  int delay = record->ping_pong.elapsed ();

  record->ping_pong.setHMS (99, 99, 99);

  for_all (_clients,
           [&] (ClientRecord* r)
           {
             if (!r->active)
               return;

             r->client->ping_pong_record (record->player.get_uuid (), delay);
           });
}

void DnDServer::ping_pong_timeout ()
{
  for_all (_clients,
           [] (ClientRecord* r)
           {
             if (!r->active || r->ping_pong.isValid ())
               return;

             r->ping_pong.restart ();
             r->client->ping_pong ();
           });
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
