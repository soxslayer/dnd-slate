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
#include <QThread>
#include <QDebug>

#include "slate_window.h"
#include "connect_dialog.h"
#include "chat_widget.h"
#include "player_list.h"
#include "game_board.h"
#include "tile_select_dialog.h"
#include "tile.h"
#include "custom_tile.h"
#include "image_tile.h"
#include "command.h"
#include "command_param.h"
#include "command_param_list.h"
#include "command_manager.h"

SlateWindow::SlateWindow ()
{
  QMenuBar* menu_bar = menuBar ();

  QMenu* menu = menu_bar->addMenu ("&File");

  _open_action = menu->addAction ("&Open");
  _open_action->setEnabled (false);
  connect (_open_action, SIGNAL (triggered (bool)),
           SLOT (open_triggered (bool)));

  QAction* action = menu->addAction ("&Quit");
  connect (action, SIGNAL (triggered (bool)),
           SLOT (quit_triggered (bool)));

  menu = menu_bar->addMenu ("&Connection");

  _connect_action = menu->addAction ("&Connect");
  connect (_connect_action, SIGNAL (triggered (bool)),
           SLOT (connect_triggered (bool)));

  _disconnect_action = menu->addAction ("&Disconnect");
  _disconnect_action->setEnabled (false);
  connect (_disconnect_action, SIGNAL (triggered (bool)),
           SLOT (disconnect_triggered (bool)));

  _status_label = new QLabel ("Disconnected", statusBar ());
  statusBar ()->insertWidget (0, _status_label);

  menu = menu_bar->addMenu ("&Map");

  _add_tile_action = menu->addAction ("&Add Tile");
  _add_tile_action->setEnabled (false);
  connect (_add_tile_action, SIGNAL (triggered (bool)),
           SLOT (add_tile_triggered (bool)));

  _delete_tile_action = menu->addAction ("&Delete Tile");
  _delete_tile_action->setEnabled (false);
  connect (_delete_tile_action, SIGNAL (triggered (bool)),
           SLOT (delete_tile_triggered (bool)));

  QFrame* main_frame = new QFrame (this);
  main_frame->setFrameStyle (0);

  QGridLayout* main_layout = new QGridLayout (main_frame);
  main_frame->setLayout (main_layout);

  _chat_widget = new ChatWidget (main_frame);
  _chat_widget->setEnabled (false);
  main_layout->addWidget (_chat_widget, 1, 0, 1, 2);
  connect (_chat_widget,
           SIGNAL (send_message (const QString&, const QString&)),
           SLOT (send_message (const QString&, const QString&)));

  _player_list = new PlayerList (this);
  main_layout->addWidget (_player_list, 0, 1);
  connect (_player_list, SIGNAL (player_activated (const PlayerPointer&)),
           SLOT (player_activated (const PlayerPointer&)));

  _board = new GameBoard (this);
  main_layout->addWidget (_board, 0, 0);
  connect (_board, SIGNAL (tile_moved (const TilePointer&, int, int)),
           SLOT (tile_moved (const TilePointer&, int, int)));

  setCentralWidget (main_frame);

  _controller_thread = new QThread (this);
  _controller_thread->start ();

  _controller = new DnDController;
  _controller->moveToThread (_controller_thread);

  connect (_controller, SIGNAL (server_connected ()),
           SLOT (server_connected ()));
  connect (_controller, SIGNAL (server_disconnected ()),
           SLOT (server_disconnected ()));
  connect (_controller, SIGNAL (player_connected (const PlayerPointer&)),
           SLOT (player_connected (const PlayerPointer&)));
  connect (_controller, SIGNAL (player_disconnected (const PlayerPointer&)),
           SLOT (player_disconnected (const PlayerPointer&)));
  connect (_controller, SIGNAL (server_message (const QString&, int, bool)),
           SLOT (server_message (const QString&, int, bool)));
  connect (_controller,
    SIGNAL (chat_message (const PlayerPointer&, const PlayerPointer&,
                          const QString&)),
    SLOT (chat_message (const PlayerPointer&, const PlayerPointer&,
                        const QString&)));
  connect (_controller,
           SIGNAL (map_changed (const ImagePointer&)),
           SLOT (map_changed (const ImagePointer&)));
  connect (_controller, SIGNAL (tile_added (const TilePointer&)),
           _board, SLOT (add_tile (const TilePointer&)));
  connect (_controller, SIGNAL (tile_moved (const TilePointer&)),
           _board, SLOT (update_tile (const TilePointer&)));
  connect (_controller, SIGNAL (tile_deleted (const TilePointer&)),
           _board, SLOT (delete_tile (const TilePointer&)));
}

SlateWindow::~SlateWindow ()
{
  _controller_thread->quit ();
  _controller_thread->wait ();
}

void SlateWindow::open_triggered (bool)
{
  QFileDialog f_diag (this);
  f_diag.setFileMode (QFileDialog::ExistingFile);

  if (f_diag.exec ()) {
    QStringList selected_files = f_diag.selectedFiles ();
    QString file = *(selected_files.begin ());

    _controller->load_map (file);
  }
}

void SlateWindow::quit_triggered (bool)
{
  QCoreApplication::quit ();
}

void SlateWindow::connect_triggered (bool)
{
  ConnectDialog diag (this);

  if (diag.exec ()) {
    _controller->connect (diag.get_name (), diag.get_host (),
                          diag.get_port ().toShort ());
  }
}

void SlateWindow::disconnect_triggered (bool)
{
  _controller->disconnect ();
}

void SlateWindow::add_tile_triggered (bool)
{
  TileSelectDialog diag (this);

  if (diag.exec ()) {
    switch (diag.get_selection_type ()) {
      case TileSelectDialog::SelectLoad:
        _controller->add_tile (Tile::TILE_IMAGE, 0, 0, 0, 0,
                               diag.get_filename ());
        break;

      case TileSelectDialog::SelectCustom:
        _controller->add_tile (Tile::TILE_CUSTOM, 0, 0, diag.get_width (),
                               diag.get_height (), diag.get_text ());
        break;
    }
  }
}

void SlateWindow::player_activated (const PlayerPointer& player)
{
  QString n_entry = player->get_name ();
  n_entry.prepend ("/w ");
  n_entry.append (" ");
  _chat_widget->set_entry (n_entry);
}

void SlateWindow::tile_moved (const TilePointer& tile, int x, int y)
{
  _controller->move_tile (tile, x, y);
}

void SlateWindow::delete_tile_triggered (bool)
{
  TilePointer tile = _board->get_selected ();

  if (tile)
    _controller->delete_tile (tile);
}

void SlateWindow::send_message (const QString& who, const QString& message)
{
  PlayerPointer who_ptr = _controller->lookup_player (who);
  _controller->chat_message (who_ptr, message);
}

void SlateWindow::server_connected ()
{
  _open_action->setEnabled (true);
  _connect_action->setEnabled (false);
  _disconnect_action->setEnabled (true);
  _add_tile_action->setEnabled (true);
  _delete_tile_action->setEnabled (true);
  _chat_widget->setEnabled (true);

  _status_label->setText ("Connected");
}

void SlateWindow::server_disconnected ()
{
  _open_action->setEnabled (false);
  _connect_action->setEnabled (true);
  _disconnect_action->setEnabled (false);
  _add_tile_action->setEnabled (false);
  _delete_tile_action->setEnabled (false);
  _chat_widget->setEnabled (false);
  _player_list->clear ();
  _board->clear_map ();

  _status_label->setText ("Disconnected");
  _chat_widget->insert_status ("Server closed");
}

void SlateWindow::player_connected (const PlayerPointer& player)
{
  QString status = player->get_name ();
  status += " connected";

  _player_list->add_player (player);
  _chat_widget->insert_status (status);
}

void SlateWindow::player_disconnected (const PlayerPointer& player)
{
  QString status = player->get_name ();
  status += " disconnected";

  _player_list->remove_player (player);
  _chat_widget->insert_status (status);
}

void SlateWindow::server_message (const QString& msg, int level, bool)
{
  if (level == DnDController::MessageError)
    QMessageBox::critical (this, "Error", msg);
  else if (level == DnDController::MessageWarn)
    QMessageBox::warning (this, "Warning", msg);
  else if (level == DnDController::MessageInfo)
    QMessageBox::information (this, "Info", msg);
}

void SlateWindow::chat_message (const PlayerPointer& sender,
                                const PlayerPointer& receiver,
                                const QString& message)
{
  int chat_widget_flags = 0;
  PlayerPointer who = sender;

  if (sender && receiver) {
    chat_widget_flags |= ChatWidget::ChatWhisper;

    if (sender->get_me ()) {
      who = receiver;
      chat_widget_flags |= ChatWidget::ChatFromMe;
    }
  }

  _chat_widget->insert_message (who, message, chat_widget_flags);
}

void SlateWindow::map_changed (const ImagePointer& image)
{
  _board->set_map (image);
}
