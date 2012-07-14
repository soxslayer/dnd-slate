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

#include <QtGlobal>
#include <QMenuBar>
#include <QMenu>
#include <QCoreApplication>
#include <QStatusBar>
#include <QLabel>
#include <QFrame>
#include <QGridLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>

#include "slate_window.h"
#include "connect_dialog.h"
#include "dnd_server.h"
#include "dnd_client.h"
#include "dnd_messages.h"
#include "chat_widget.h"
#include "player_list.h"
#include "game_board.h"
#include "tile_select_dialog.h"
#include "tile.h"
#include "custom_tile.h"
#include "image_tile.h"

SlateWindow::SlateWindow ()
  : _server (0)
{
  QMenuBar* menu_bar = menuBar ();

  QMenu* menu = menu_bar->addMenu ("&File");

  _open_action = menu->addAction ("&Open");
  _open_action->setEnabled (false);
  connect (_open_action, SIGNAL (triggered (bool)),
           this, SLOT (open_triggered (bool)));

  QAction* action = menu->addAction ("&Quit");
  connect (action, SIGNAL (triggered (bool)),
           this, SLOT (quit_triggered (bool)));

  menu = menu_bar->addMenu ("&Connection");

  _connect_action = menu->addAction ("&Connect");
  connect (_connect_action, SIGNAL (triggered (bool)),
           this, SLOT (connect_triggered (bool)));

  _disconnect_action = menu->addAction ("&Disconnect");
  _disconnect_action->setEnabled (false);
  connect (_disconnect_action, SIGNAL (triggered (bool)),
           this, SLOT (disconnect_triggered (bool)));

  _status_label = new QLabel ("Disconnected", statusBar ());
  statusBar ()->insertWidget (0, _status_label);

  menu = menu_bar->addMenu ("&Map");

  _add_tile_action = menu->addAction ("&Add Tile");
  _add_tile_action->setEnabled (false);
  connect (_add_tile_action, SIGNAL (triggered (bool)),
           this, SLOT (add_tile_triggered (bool)));

  _delete_tile_action = menu->addAction ("&Delete Tile");
  _delete_tile_action->setEnabled (false);
  connect (_delete_tile_action, SIGNAL (triggered (bool)),
           this, SLOT (delete_tile_triggered (bool)));

  QFrame* main_frame = new QFrame (this);
  main_frame->setFrameStyle (0);

  QGridLayout* main_layout = new QGridLayout (main_frame);
  main_frame->setLayout (main_layout);

  _chat_widget = new ChatWidget (main_frame);
  _chat_widget->setEnabled (false);
  main_layout->addWidget (_chat_widget, 1, 0, 1, 2);
  connect (_chat_widget,
           SIGNAL (send_message (const QString&, const QString&)),
           this, SLOT (send_message (const QString&, const QString&)));

  _player_list = new PlayerList (this);
  main_layout->addWidget (_player_list, 0, 1);
  connect (_player_list, SIGNAL (player_activated (Uuid)),
           this, SLOT (player_activated (Uuid)));

  _board = new GameBoard (this);
  main_layout->addWidget (_board, 0, 0);
  connect (_board, SIGNAL (tile_moved (Uuid, int, int)),
           this, SLOT (tile_moved (Uuid, int, int)));

  setCentralWidget (main_frame);
}

void SlateWindow::open_triggered (bool checked)
{
  (void)checked;

  QFileDialog f_diag (this);
  f_diag.setFileMode (QFileDialog::ExistingFile);

  if (f_diag.exec ()) {
    QStringList selected_files = f_diag.selectedFiles ();
    QStringList::iterator file = selected_files.begin ();

    _client->load_image (*file);
  }
}

void SlateWindow::quit_triggered (bool checked)
{
  (void)checked;

  QCoreApplication::quit ();
}

void SlateWindow::connect_triggered (bool checked)
{
  (void)checked;

  ConnectDialog diag (this);

  if (diag.exec ()) {
    QString host = diag.get_host ();
    QString port = diag.get_port ();
    _name = diag.get_name ();

    if (host.isEmpty ()) {
      _server = new DnDServer (port.toShort ());
      host = "localhost";
    }

    _client = new DnDClient (host, port.toShort ());

    connect (_client, SIGNAL (comm_proto_resp (DnDClient*, quint16, quint16)),
             this, SLOT (comm_proto_resp (DnDClient*, quint16, quint16)));
    connect (_client, SIGNAL (server_message (DnDClient*, const QString&,
                              int)),
             this, SLOT (server_message (DnDClient*, const QString&, int)));
    connect (_client, SIGNAL (user_add_resp (DnDClient*, Uuid,
                              const QString&)),
             this, SLOT (user_add_resp (DnDClient*, Uuid,
                         const QString&)));
    connect (_client, SIGNAL (user_del (DnDClient*, Uuid)),
             this, SLOT (user_del (DnDClient*, Uuid)));
    connect (_client,
      SIGNAL (chat_message (DnDClient*, Uuid, Uuid, const QString&, int)),
      this,
      SLOT (chat_message (DnDClient*, Uuid, Uuid, const QString&, int)));
    connect (_client, SIGNAL (image_begin (DnDClient*, quint32, quint32)),
             this, SLOT (map_begin (DnDClient*, quint32, quint32)));
    connect (_client, SIGNAL (image_data (DnDClient*, quint32, quint32,
                              const uchar*, quint64)),
             this, SLOT (map_data (DnDClient*, quint32, quint32,
                         const uchar*, quint64)));
    connect (_client, SIGNAL (image_end (DnDClient*, quint32)),
             this, SLOT (map_end (DnDClient*, quint32)));
    connect (_client, SIGNAL (add_tile (DnDClient*, Uuid, quint8, quint16,
                              quint16, quint16, quint16, const QString&)),
             this, SLOT (add_tile (DnDClient*, Uuid, quint8, quint16,
                         quint16, quint16, quint16, const QString&)));
    connect (_client, SIGNAL (move_tile (DnDClient*, Uuid, Uuid,
                              quint16, quint16)),
             this, SLOT (move_tile (DnDClient*, Uuid, Uuid,
                         quint16, quint16)));
    connect (_client, SIGNAL (delete_tile (DnDClient*, Uuid, Uuid)),
             this, SLOT (delete_tile (DnDClient*, Uuid, Uuid)));
    connect (_client, SIGNAL (connected ()),
             this, SLOT (server_connected ()));
    connect (_client, SIGNAL (disconnected ()),
             this, SLOT (server_disconnected ()));
  }
}

void SlateWindow::disconnect_triggered (bool checked)
{
  (void)checked;

  disconnect_client ();
  delete _client;
}

void SlateWindow::add_tile_triggered (bool checked)
{
  (void)checked;

  TileSelectDialog diag (this);

  if (diag.exec ()) {
    switch (diag.get_selection_type ()) {
      case TileSelectDialog::SelectLoad:
        _client->add_tile (_player_list->get_my_uuid (), Tile::TILE_IMAGE, 0,
                           0, 0, 0, diag.get_filename ());
        break;

      case TileSelectDialog::SelectCustom:
        _client->add_tile (_player_list->get_my_uuid (), Tile::TILE_CUSTOM, 0,
                           0, diag.get_width (), diag.get_height (),
                           diag.get_text ());
        break;
    }
  }
}

void SlateWindow::delete_tile_triggered (bool checked)
{
  (void)checked;

  Uuid selected_uuid = _board->get_selected_uuid ();

  if (selected_uuid != UuidManager::UUID_INVALID)
    _client->delete_tile (_player_list->get_my_uuid (), selected_uuid);
}

void SlateWindow::server_connected ()
{
  _client->comm_proto_req ();
}

void SlateWindow::server_disconnected ()
{
  _chat_widget->insert_status ("Server closed");
  disconnect_client ();
  _client->deleteLater ();
}

void SlateWindow::send_message (const QString& who, const QString& message)
{
  Uuid src = _player_list->get_my_uuid ();
  Uuid dst;
  int flags = 0;

  if (!who.isEmpty ())
    dst = _player_list->get_player_uuid (who);
  else
    flags |= CHAT_BROADCAST;

  _client->chat_message (src, dst, message, flags);
}

void SlateWindow::comm_proto_resp (DnDClient* client, quint16 major,
                                   quint16 minor)
{
  (void)client;

  if (major != COMM_PROTO_MAJOR || minor != COMM_PROTO_MINOR) {
    QMessageBox::critical (this, "Error",
      "Incompatible communication protocol with server");

    disconnect_client ();
  }
  else {
    _client->user_add_req (_name);

    _open_action->setEnabled (true);
    _connect_action->setEnabled (false);
    _disconnect_action->setEnabled (true);
    _add_tile_action->setEnabled (true);
    _delete_tile_action->setEnabled (true);
    _chat_widget->setEnabled (true);

    _status_label->setText ("Connected");
  }
}

void SlateWindow::server_message (DnDClient* client, const QString& msg,
                                  int flags)
{
  (void)client;

  if (flags & MESSAGE_ERROR)
    QMessageBox::critical (this, "Error", msg);
  else if (flags & MESSAGE_WARN)
    QMessageBox::warning (this, "Warning", msg);
  else if (flags & MESSAGE_INFO)
    QMessageBox::information (this, "Warning", msg);
}

void SlateWindow::user_add_resp (DnDClient* client, Uuid uuid,
                                 const QString& name)
{
  (void)client;

  QString status = name;
  status += " connected";

  _player_list->add_player (uuid, name);
  _chat_widget->insert_status (status);
}

void SlateWindow::user_del (DnDClient* client, Uuid uuid)
{
  (void)client;

  QString status = _player_list->get_player_name (uuid);
  status += " disconnected";

  _player_list->remove_player (uuid);
  _chat_widget->insert_status (status);
}

void SlateWindow::chat_message (DnDClient* client, Uuid src, Uuid dst,
                                const QString& message, int flags)
{
  (void)client;

  int chat_widget_flags = 0;
  QString who;

  if (!(flags & CHAT_BROADCAST))
    chat_widget_flags |= ChatWidget::ChatWhisper;

  if (src == _player_list->get_my_uuid ())
    chat_widget_flags |= ChatWidget::ChatFromMe;

  if (chat_widget_flags & ChatWidget::ChatWhisper
      && chat_widget_flags & ChatWidget::ChatFromMe)
    who = _player_list->get_player_name (dst);
  else
    who = _player_list->get_player_name (src);

  _chat_widget->insert_message (message, who, chat_widget_flags);
}

void SlateWindow::map_begin (DnDClient* client, quint32 size, quint32 id)
{
  (void)client;

  _map_transfer_id = id;
  _map_buff = new QByteArray (size, 0);
}

void SlateWindow::map_data (DnDClient* client, quint32 id, quint32 sequence,
                            const uchar* data, quint64 size)
{
  (void)client;

  if (id != _map_transfer_id)
    return;

  _map_buff->insert (sequence * DND_IMAGE_MAX_CHUNK_SIZE, (char*)data, size);
}

void SlateWindow::map_end (DnDClient* client, quint32 id)
{
  (void)client;

  if (id != _map_transfer_id)
    return;

  QImage* bg_image = new QImage;
  bg_image->loadFromData (*_map_buff);
  _board->set_map (bg_image);

  delete _map_buff;
}

void SlateWindow::add_tile (DnDClient* client, Uuid uuid, quint8 type,
                            quint16 x, quint16 y, quint16 w, quint16 h,
                            const QString& text)
{
  (void)client;

  GameTile* tile;

  switch (type) {
    case Tile::TILE_CUSTOM: {
      tile = new CustomTile (uuid, w, h, text);

      break;
    }

    case Tile::TILE_IMAGE: {
      QString path = text;
      path.prepend ("image:");
      QDir tile_path (path);

      tile = new ImageTile (uuid, tile_path.absolutePath ());

      break;
    }
  }

  _board->add_tile (tile);
  tile->set_x (x);
  tile->set_y (y);
}

void SlateWindow::move_tile (DnDClient* client, Uuid player_uuid,
                             Uuid tile_uuid, quint16 x, quint16 y)
{
  (void)client;
  (void)player_uuid;

  _board->move_tile (tile_uuid, x, y);
}

void SlateWindow::delete_tile (DnDClient* client, Uuid player_uuid,
                               Uuid tile_uuid)
{
  (void)client;
  (void)player_uuid;

  qDebug () << "Delete tile";
  _board->delete_tile (tile_uuid);
}

void SlateWindow::player_activated (Uuid uuid)
{
  QString n_entry = _player_list->get_player_name (uuid);
  n_entry.prepend ("/w ");
  n_entry.append (" ");
  _chat_widget->set_entry (n_entry);
}

void SlateWindow::tile_moved (Uuid uuid, int x, int y)
{
  _client->move_tile (_player_list->get_my_uuid (), uuid, x, y);
}

void SlateWindow::disconnect_client ()
{
  _open_action->setEnabled (false);
  _connect_action->setEnabled (true);
  _disconnect_action->setEnabled (false);
  _add_tile_action->setEnabled (false);
  _delete_tile_action->setEnabled (false);
  _chat_widget->setEnabled (false);
  _player_list->clear ();
  _board->clear_map ();

  disconnect (_client, 0, 0, 0);

  if (_server)
    delete _server;

  _status_label->setText ("Disconnected");
}
