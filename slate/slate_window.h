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

#ifndef __SLATE_WINDOW__
#define __SLATE_WINDOW__

#include <QMainWindow>
#include <QString>

#include "uuid.h"
#include "command.h"

class QAction;
class QLabel;
class DnDServer;
class DnDClient;
class ChatWidget;
class PlayerList;
class GameBoard;
class QByteArray;
class CommandParamList;

class SlateWindow : public QMainWindow
{
  Q_OBJECT;

public:
  SlateWindow ();
  ~SlateWindow () { }

  bool connect_command (const CommandParamList& params);
  bool disconnect_command (const CommandParamList& params);

private slots:
  void open_triggered (bool checked);
  void quit_triggered (bool checked);
  void connect_triggered (bool checked);
  void disconnect_triggered (bool checked);
  void add_tile_triggered (bool checked);
  void delete_tile_triggered (bool checked);
  void server_connected ();
  void server_disconnected ();
  void send_message (const QString& who, const QString& message);
  void comm_proto_resp (DnDClient* client, quint16 major, quint16 minor);
  void server_message (DnDClient* client, const QString& msg, int flags);
  void user_add_resp (DnDClient* client, Uuid uuid, const QString& name);
  void user_del (DnDClient* client, Uuid);
  void chat_message (DnDClient* client, Uuid src, Uuid dst,
                     const QString& message, int flags);
  void map_begin (DnDClient* client, quint32 size, quint32 id);
  void map_data (DnDClient* client, quint32 id, quint32 sequence,
                 const uchar* data, quint64 size);
  void map_end (DnDClient* client, quint32 id);
  void add_tile (DnDClient* client, Uuid uuid, quint8 type, quint16 x,
                 quint16 y, quint16 w, quint16 h, const QString& text);
  void move_tile (DnDClient* client, Uuid player_uuid, Uuid tile_uuid,
                  quint16 x, quint16 y);
  void delete_tile (DnDClient* client, Uuid player_uuid, Uuid tile_uuid);
  void ping_pong (DnDClient* client, Uuid uuid);
  void ping_pong_record (DnDClient* client, Uuid uuid, quint32 delay);
  void player_activated (Uuid uuid);
  void tile_moved (Uuid uuid, int x, int y);

private:
  QAction* _open_action;
  QAction* _connect_action;
  QAction* _disconnect_action;
  QAction* _add_tile_action;
  QAction* _delete_tile_action;
  QLabel* _status_label;
  ChatWidget* _chat_widget;
  PlayerList* _player_list;
  DnDServer* _server;
  DnDClient* _client;
  QString _name;
  GameBoard* _board;
  QByteArray* _map_buff;
  quint32 _map_transfer_id;
  Command<SlateWindow> _connect_command;
  Command<SlateWindow> _disconnect_command;

  void disconnect_client ();
  bool connect_client (const QString& host, quint16 port,
                       const QString& name);
};

#endif /* __SLATE_WINDOW__ */
