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

#ifndef __DND_SERVER__
#define __DND_SERVER__

#include <QTcpServer>
#include <QMap>
#include <QByteArray>

#include "uuid.h"

class DnDClient;
class Tile;

class DnDServer : public QTcpServer
{
  Q_OBJECT

public:
  DnDServer (quint16 port);
  ~DnDServer ();

protected:
  virtual void incomingConnection (int socketDescriptor);

private slots:
  void client_disconnected (DnDClient* client);
  void client_comm_proto_req (DnDClient* client);
  void client_user_add_req (DnDClient* client, const QString& name);
  void client_chat_message (DnDClient* client, Uuid src, Uuid dst,
                            const QString& msg, int flags);
  void client_load_image (DnDClient* client, const QString& file_name);
  void client_add_tile (DnDClient* client, Uuid uuid, quint8 type,
                        quint16 x, quint16 y, quint16 w, quint16 h,
                        const QString& text);
  void client_move_tile (DnDClient* client, Uuid player_uuid,
                         Uuid tile_uuid, quint16 x, quint16 y);
  void client_delete_tile (DnDClient* client, Uuid player_uuid,
                           Uuid tile_uuid);

private:
  struct ClientId
  {
    ClientId (const QString& name, DnDClient* client)
      : name (name), client (client) { }
    QString name;
    DnDClient* client;
  };

  UuidManager _uuid_manager;
  Uuid _dm_uuid;
  DnDClient* _dm_client;
  QByteArray _map_data;
  QMap<Uuid, ClientId*> _client_map;
  QMap<Uuid, Tile*> _tile_map;

  void send_map (DnDClient* client);
};

#endif /* __DND_SERVER__ */
