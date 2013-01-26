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

#ifndef __SLATE_WINDOW__
#define __SLATE_WINDOW__

#include <QMainWindow>
#include <QString>

#include "uuid.h"
#include "player.h"
#include "dnd_controller.h"
#include "image.h"

class QAction;
class QLabel;
class QThread;
class ChatWidget;
class PlayerList;
class GameBoard;
class CommandParamList;

class SlateWindow : public QMainWindow
{
  Q_OBJECT;

public:
  SlateWindow ();
  ~SlateWindow ();

  bool connect_command (const CommandParamList& params);
  bool disconnect_command (const CommandParamList& params);

private slots:
  void open_triggered (bool checked);
  void quit_triggered (bool checked);
  void connect_triggered (bool checked);
  void disconnect_triggered (bool checked);
  void add_tile_triggered (bool checked);
  void player_activated (const PlayerPointer& player);
  void tile_moved (const TilePointer& tile, int x, int y);
  void delete_tile_triggered (bool checked);
  void send_message (const QString& who, const QString& message);
  void server_connected ();
  void server_disconnected ();
  void player_connected (const PlayerPointer& player);
  void player_disconnected (const PlayerPointer& player);
  void server_message (const QString& msg, int level, bool internal);
  void chat_message (const PlayerPointer& sender,
                     const PlayerPointer& receiver,
                     const QString& message);
  void map_changed (const ImagePointer& image);

private:
  QAction* _open_action;
  QAction* _connect_action;
  QAction* _disconnect_action;
  QAction* _add_tile_action;
  QAction* _delete_tile_action;
  QLabel* _status_label;
  ChatWidget* _chat_widget;
  PlayerList* _player_list;
  GameBoard* _board;
  QThread* _controller_thread;
  DnDController* _controller;

  void disconnect_client ();
};

#endif /* __SLATE_WINDOW__ */
