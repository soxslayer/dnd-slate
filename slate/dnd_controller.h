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

#ifndef __DND_CONTROLLER__
#define __DND_CONTROLLER__

#include <QObject>
#include <QString>
#include <QSharedPointer>
#include <QMap>

#include "non_copyable.h"
#include "uuid.h"
#include "tile.h"
#include "command.h"
#include "player.h"

class QByteArray;
class DnDServer;
class DnDClient;
class CommandParamList;

typedef QSharedPointer<QByteArray> ImageDataPointer;

class DnDController : public QObject, public NonCopyable
{
  Q_OBJECT

public:
  typedef enum
  {
    MessageInfo,
    MessageWarn,
    MessageError
  } MessageFlags;

  DnDController ();
  ~DnDController ();

  PlayerPointer lookup_player (const QString& name);

public slots:
  void connect (const QString& user_name, const QString& host, quint16 port);
  void disconnect ();
  void chat_message (const PlayerPointer& receiver,
                     const QString& message);
  void set_map (const QString& file);
  void add_tile (Tile::TileType type, quint16 x, quint16 y, quint16 w,
                 quint16 h, const QString& text);
  void move_tile (const TilePointer& tile, quint16 x, quint16 y);
  void delete_tile (const TilePointer& tile);

signals:
  void server_connected ();
  void server_disconnected ();
  void player_connected (const PlayerPointer& player);
  void player_disconnected (const PlayerPointer& player);
  void server_message (const QString& message, int level, bool internal);
  void chat_message (const PlayerPointer& sender,
                     const PlayerPointer& receiver,
                     const QString& message);
  void map_updated (const ImageDataPointer& data);
  void tile_added (const TilePointer& tile);
  void tile_moved (const TilePointer& tile);
  void tile_deleted (const TilePointer& tile);

private slots:
  void comm_proto_resp (DnDClient* client, quint16 major, quint16 minor);
  void server_message (DnDClient* client, const QString& msg, int flags);
  void user_add_resp (DnDClient* client, Uuid uuid, const QString& name);
  void user_del (DnDClient* client, Uuid uuid);
  void chat_message (DnDClient* client, Uuid src_uuid, Uuid dst_uuid,
                     const QString& message, int flags);
  void image_begin (DnDClient* client, quint32 size, quint32 id);
  void image_data (DnDClient* client, quint32 id, quint32 seq,
                   const uchar* data, quint32 size);
  void image_end (DnDClient* client, quint32 id);
  void add_tile (DnDClient* client, Uuid tile_uuid, quint8 type, quint16 x,
                 quint16 y, quint16 w, quint16 h, const QString& text);
  void move_tile (DnDClient* client, Uuid tile_uuid, quint16 x, quint16 y);
  void delete_tile (DnDClient* client, Uuid tile_uuid);
  void ping_pong (DnDClient* client);

private:
  typedef QMap<quint32, ImageDataPointer> ImageTransferMap;
  typedef QMap<Uuid, TilePointer> TileMap;
  typedef QMap<Uuid, PlayerPointer> PlayerMap;

  DnDServer* _server;
  DnDClient* _client;
  QString _name;
  ImageTransferMap _image_transfer_map;
  TileMap _tile_map;
  PlayerMap _player_map;
  PlayerPointer _me;
  Command<DnDController> _connect_cmd;
  Command<DnDController> _disconnect_cmd;
  Command<DnDController> _set_map_cmd;
  Command<DnDController> _add_tile_cmd;

  bool connect_cmd (const CommandParamList& params);
  bool disconnect_cmd (const CommandParamList& params);
  bool set_map_cmd (const CommandParamList& params);
  bool add_tile_cmd (const CommandParamList& params);
};

#endif /* __DND_CONTROLLER__ */
