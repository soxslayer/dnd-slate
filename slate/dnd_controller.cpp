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

#include "dnd_controller.h"
#include "dnd_server.h"
#include "dnd_client.h"
#include "dnd_messages.h"
#include "player_list.h"
#include "tile.h"
#include "command_manager.h"
#include "command_param_list.h"
#include "command_param.h"

DnDController::DnDController ()
  : _connect_cmd (this, &DnDController::connect_cmd),
    _disconnect_cmd (this, &DnDController::disconnect_cmd),
    _set_map_cmd (this, &DnDController::set_map_cmd),
    _add_tile_cmd (this, &DnDController::add_tile_cmd)
{
  CommandManager::add_command ("connect", &_connect_cmd);
  CommandManager::add_command ("disconnect", &_disconnect_cmd);
  CommandManager::add_command ("set_map", &_set_map_cmd);
  CommandManager::add_command ("add_tile", &_add_tile_cmd);
}

DnDController::~DnDController ()
{
  if (_client)
    delete _client;

  if (_server)
    delete _server;
}

PlayerPointer DnDController::lookup_player (const QString& name)
{
  return [&] () -> PlayerPointer {
    auto b = _player_map.begin ();
    auto e = _player_map.end ();

    for (; b != e; ++b) {
      if ((*b)->get_name () == name)
        return *b;
    }

    return PlayerPointer ();
  } ();
}

void DnDController::connect (const QString& user_name, const QString& host,
                             quint16 port)
{
  QString my_host = host;

  if (host.isEmpty ()) {
    _server = new DnDServer (port);
    my_host = "localhost";
  }

  _client = new DnDClient (my_host, port);

  _name = user_name;

  QObject::connect (_client,
                    SIGNAL (comm_proto_resp (DnDClient*, quint16, quint16)),
                    SLOT (comm_proto_resp (DnDClient*, quint16, quint16)));
  QObject::connect (_client,
                    SIGNAL (server_message (DnDClient*, const QString&, int)),
                    SLOT (server_message (DnDClient*, const QString&, int)));
  QObject::connect (_client,
                    SIGNAL (user_add_resp (DnDClient*, Uuid, const QString&)),
                    SLOT (user_add_resp (DnDClient*, Uuid, const QString&)));
  QObject::connect (_client,
                    SIGNAL (user_del (DnDClient*, Uuid)),
                    SLOT (user_del (DnDClient*, Uuid)));
  QObject::connect (_client,
    SIGNAL (chat_message (DnDClient*, Uuid, Uuid, const QString&, int)),
    SLOT (chat_message (DnDClient*, Uuid, Uuid, const QString&, int)));
  QObject::connect (_client,
    SIGNAL (image_begin (DnDClient*, quint32, quint32)),
    SLOT (image_begin (DnDClient*, quint32, quint32)));
  QObject::connect (_client,
    SIGNAL (image_data (DnDClient*, quint32, quint32, const uchar*, quint32)),
    SLOT (image_data (DnDClient*, quint32, quint32, const uchar*, quint32)));
  QObject::connect (_client, SIGNAL (image_end (DnDClient*, quint32)),
    SLOT (image_end (DnDClient*, quint32)));
  QObject::connect (_client,
    SIGNAL (add_tile (DnDClient*, Uuid, quint8, quint16, quint16, quint16,
            quint16, const QString&)),
    SLOT (add_tile (DnDClient*, Uuid, quint8, quint16, quint16, quint16,
            quint16, const QString&)));
  QObject::connect (_client,
    SIGNAL (move_tile (DnDClient*, Uuid, quint16, quint16)),
    SLOT (move_tile (DnDClient*, Uuid, quint16, quint16)));
  QObject::connect (_client, SIGNAL (delete_tile (DnDClient*, Uuid)),
    SLOT (delete_tile (DnDClient*, Uuid)));
  QObject::connect (_client, SIGNAL (ping_pong (DnDClient*)),
    SLOT (ping_pong (DnDClient*)));

  _client->comm_proto_req ();
}

void DnDController::disconnect ()
{
  if (!_client)
    return;

  delete _client;

  if (!_server)
    delete _server;

  _player_map.clear ();

  server_disconnected ();
}

void DnDController::chat_message (const PlayerPointer& receiver,
                                  const QString& message)
{
  int flags = 0;
  Uuid dst_uuid;

  if (receiver)
    dst_uuid = receiver->get_uuid ();
  else
    flags |= CHAT_BROADCAST;

  _client->chat_message (_me->get_uuid (), dst_uuid, message, flags);
}

void DnDController::set_map (const QString& file)
{
  _client->load_image (file);
}

void DnDController::add_tile (Tile::TileType type, quint16 x, quint16 y,
                              quint16 w, quint16 h, const QString& text)
{
  _client->add_tile (0, type, x, y, w, h, text);
}

void DnDController::move_tile (const TilePointer& tile, quint16 x, quint16 y)
{
  if (_tile_map.count (tile->get_uuid ()) == 0) {
    server_message ("Invalid tile", MESSAGE_ERROR, true);
    return;
  }

  _client->move_tile (tile->get_uuid (), x, y);
}

void DnDController::delete_tile (const TilePointer& tile)
{
  if (_tile_map.count (tile->get_uuid ()) == 0) {
    server_message ("Invalid tile", MESSAGE_ERROR, true);
    return;
  }

  _client->delete_tile (tile->get_uuid ());
}

void DnDController::comm_proto_resp (DnDClient*, quint16 major, quint16 minor)
{
  if (major != COMM_PROTO_MAJOR || minor != COMM_PROTO_MINOR) {
    server_message ("Incompatible communication protocol with server",
                    MESSAGE_ERROR, true);
    disconnect ();

    return;
  }

  server_connected ();
  _client->user_add_req (_name);
}

void DnDController::server_message (DnDClient*, const QString& msg, int flags)
{
  int level;

  if (flags & MESSAGE_ERROR)
    level = MessageError;
  else if (flags & MESSAGE_WARN)
    level = MessageWarn;
  else
    level = MessageInfo;

  server_message (msg, level, false);
}

void DnDController::user_add_resp (DnDClient*, Uuid uuid, const QString& name)
{
  bool is_me = _player_map.isEmpty ();

  _player_map.insert (uuid, PlayerPointer (new Player (uuid, name, is_me)));

  if (is_me)
    _me = _player_map[uuid];

  player_connected (_player_map[uuid]);
}

void DnDController::user_del (DnDClient*, Uuid uuid)
{
  if (_player_map.count (uuid) == 0)
    return;

  player_disconnected (_player_map[uuid]);
  _player_map.remove (uuid);
}

void DnDController::chat_message (DnDClient*, Uuid src_uuid, Uuid dst_uuid,
                                  const QString& message, int flags)
{
  if (_player_map.count (src_uuid) == 0)
    return;

  PlayerPointer src = _player_map[src_uuid];
  PlayerPointer dst;

  if (!(flags & CHAT_BROADCAST)) {
    if (_player_map.count (dst_uuid) == 0)
      return;

    dst = _player_map[dst_uuid];
  }

  chat_message (src, dst, message);
}

void DnDController::image_begin (DnDClient*, quint32 size, quint32 id)
{
  _image_transfer_map.insert (id, ImageDataPointer (new QByteArray (size, 0)));
}

void DnDController::image_data (DnDClient*, quint32 id, quint32 seq,
                                const uchar* data, quint32 size)
{
  if (_image_transfer_map.count (id) == 0)
    return;

  quint32 offset = seq * DND_IMAGE_MAX_CHUNK_SIZE;
  quint32 end_byte = offset + size;

  /* Check for integer overflow */
  if (offset > end_byte)
    return;

  if (end_byte > (quint32)_image_transfer_map[id]->size ())
    return;

  _image_transfer_map[id]->insert (seq * DND_IMAGE_MAX_CHUNK_SIZE,
    (char*)data, size);
}

void DnDController::image_end (DnDClient*, quint32 id)
{
  if (_image_transfer_map.count (id) == 0)
    return;

  map_updated (_image_transfer_map[id]);

  _image_transfer_map.remove (id);
}

void DnDController::add_tile (DnDClient*, Uuid tile_uuid, quint8 type,
                              quint16 x, quint16 y, quint16 w, quint16 h,
                              const QString& text)
{
  _tile_map.insert (tile_uuid,
    TilePointer (new Tile (tile_uuid, text, (Tile::TileType)type, x, y, w, h))
  );

  tile_added (_tile_map[tile_uuid]);
}

void DnDController::move_tile (DnDClient*, Uuid tile_uuid, quint16 x,
                               quint16 y)
{
  if (_tile_map.count (tile_uuid) == 0)
    return;

  _tile_map[tile_uuid]->move (x, y);

  tile_moved (_tile_map[tile_uuid]);
}

void DnDController::delete_tile (DnDClient*, Uuid tile_uuid)
{
  if (_tile_map.count (tile_uuid) == 0)
    return;

  tile_deleted (_tile_map[tile_uuid]);
  _tile_map.remove (tile_uuid);
}

void DnDController::ping_pong (DnDClient*)
{
  _client->ping_pong ();
}

bool DnDController::connect_cmd (const CommandParamList& params)
{
  QString host;
  quint16 port;
  QString name;

  if (params.verify_signature ("sis")) {
    host = params.get_param (0)->get_str ().c_str ();
    port = (quint16)params.get_param (1)->get_int ();
    name = params.get_param (2)->get_str ().c_str ();
  }
  else if (params.verify_signature ("is")) {
    port = (quint16)params.get_param (0)->get_int ();
    name = params.get_param (1)->get_str ().c_str ();
  }
  else {
    qDebug () << "Invalid connect params";
    return false;
  }

  connect (name, host, port);

  return true;
}

bool DnDController::disconnect_cmd (const CommandParamList&)
{
  disconnect ();

  return true;
}

bool DnDController::set_map_cmd (const CommandParamList& params)
{
  QString file_name;

  if (params.verify_signature ("s"))
    file_name = params.get_param (0)->get_str ().c_str ();
  else {
    qDebug () << "Invalid set_map params";
    return false;
  }

  set_map (file_name);

  return true;
}

bool DnDController::add_tile_cmd (const CommandParamList& params)
{
  if (params.verify_signature ("iis")) {
    quint16 x = (quint16)params.get_param (0)->get_int ();
    quint16 y = (quint16)params.get_param (1)->get_int ();
    QString img = params.get_param (2)->get_str ().c_str ();

    add_tile (Tile::TILE_IMAGE, x, y, 0, 0, img);
  }
  else if (params.verify_signature ("iiiis")) {
    quint16 x = (quint16)params.get_param (0)->get_int ();
    quint16 y = (quint16)params.get_param (1)->get_int ();
    quint16 w = (quint16)params.get_param (2)->get_int ();
    quint16 h = (quint16)params.get_param (3)->get_int ();
    QString txt = params.get_param (4)->get_str ().c_str ();

    add_tile (Tile::TILE_CUSTOM, x, y, w, h, txt);
  }
  else {
    qDebug () << "Invalid add_tile params";
    return false;
  }

  return true;
}
