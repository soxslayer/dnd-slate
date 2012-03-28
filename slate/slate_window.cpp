#include <QMenuBar>
#include <QMenu>
#include <QDebug>
#include <QCoreApplication>
#include <QErrorMessage>
#include <QStatusBar>
#include <QLabel>
#include <QFrame>
#include <QGridLayout>

#include "slate_window.h"
#include "connect_dialog.h"
#include "dnd_server.h"
#include "dnd_client.h"
#include "dnd_messages.h"
#include "chat_widget.h"
#include "player_list.h"

SlateWindow::SlateWindow (const QString& bin_path)
  : _bin_path (bin_path),
    _server (0)
{
  QMenuBar* menu_bar = menuBar ();

  QMenu* menu = menu_bar->addMenu ("&File");

  QAction* action = menu->addAction ("&Open");
  connect (action, SIGNAL (triggered (bool)),
           this, SLOT (open_triggered (bool)));

  action = menu->addAction ("&Quit");
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

  QFrame* main_frame = new QFrame (this);
  main_frame->setFrameStyle (0);

  QGridLayout* main_layout = new QGridLayout (main_frame);
  main_frame->setLayout (main_layout);

  _chat_widget = new ChatWidget (main_frame);
  _chat_widget->setEnabled (false);
  main_layout->addWidget (_chat_widget, 1, 0);
  connect (_chat_widget,
           SIGNAL (send_message (const QString&, const QString&)),
           this, SLOT (send_message (const QString&, const QString&)));

  _player_list = new PlayerList (this);
  main_layout->addWidget (_player_list, 0, 0);

  setCentralWidget (main_frame);
}

void SlateWindow::open_triggered (bool checked)
{
  qDebug () << "Open triggered";
}

void SlateWindow::quit_triggered (bool checked)
{
  QCoreApplication::quit ();
}

void SlateWindow::connect_triggered (bool checked)
{
  ConnectDialog diag (this);

  if (diag.exec ()) {
    QString host = diag.get_host ();
    QString port = diag.get_port ();
    _name = diag.get_name ();

    if (port.isEmpty ()) {
      QErrorMessage msg;
      msg.showMessage ("Port required");
      msg.exec ();
      return;
    }

    if (_name.isEmpty ()) {
      QErrorMessage msg;
      msg.showMessage ("Name required");
      msg.exec ();
      return;
    }

    if (host.isEmpty ()) {
      _server = new DnDServer (port.toShort ());
      host = "localhost";
    }

    _client = new DnDClient (host, port.toShort ());

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
    connect (_client, SIGNAL (connected ()),
             this, SLOT (server_connected ()));
    connect (_client, SIGNAL (disconnected ()),
             this, SLOT (server_disconnected ()));
  }
}

void SlateWindow::disconnect_triggered (bool checked)
{
  disconnect_client ();
  delete _client;
}

void SlateWindow::server_connected ()
{
  _client->user_add_req (_name);

  _connect_action->setEnabled (false);
  _disconnect_action->setEnabled (true);
  _chat_widget->setEnabled (true);

  _status_label->setText ("Connected");
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

void SlateWindow::user_add_resp (DnDClient* client, Uuid uuid,
                                 const QString& name)
{
  QString status = name;
  status += " connected";

  _player_list->add_player (uuid, name);
  _chat_widget->insert_status (status);
}

void SlateWindow::user_del (DnDClient* client, Uuid uuid)
{
  QString status = _player_list->get_player_name (uuid);
  status += " disconnected";

  _player_list->remove_player (uuid);
  _chat_widget->insert_status (status);
}

void SlateWindow::chat_message (DnDClient* client, Uuid src, Uuid dst,
                                const QString& message, int flags)
{
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

void SlateWindow::disconnect_client ()
{
  _connect_action->setEnabled (true);
  _disconnect_action->setEnabled (false);
  _chat_widget->setEnabled (false);
  _player_list->clear ();

  disconnect (_client, 0, 0, 0);

  if (_server)
    delete _server;

  _status_label->setText ("Disconnected");
}
