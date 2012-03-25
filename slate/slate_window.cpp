#include <QMenuBar>
#include <QMenu>
#include <QDebug>
#include <QCoreApplication>
#include <QErrorMessage>
#include <QStatusBar>
#include <QLabel>

#include "slate_window.h"
#include "connect_dialog.h"
#include "dnd_server.h"
#include "dnd_client.h"
#include "chat_widget.h"

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

  _chat_widget = new ChatWidget (this);
  setCentralWidget (_chat_widget);
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

    _client_id_map.clear ();

    _client = new DnDClient (host, port.toShort ());

    connect (_client, SIGNAL (user_add_resp (DnDClient*, Uuid,
                              const QString&)),
             this, SLOT (user_add_resp (DnDClient*, Uuid,
                         const QString&)));
    connect (_client, SIGNAL (user_del (DnDClient*, Uuid)),
             this, SLOT (user_del (DnDClient*, Uuid)));
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

  _status_label->setText ("Connected");
}

void SlateWindow::server_disconnected ()
{
  _chat_widget->insert_status ("Server closed");
  disconnect_client ();
  _client->deleteLater ();
}

void SlateWindow::user_add_resp (DnDClient* client, Uuid uuid,
                                 const QString& name)
{
  QString status = name;
  status += " connected";

  _chat_widget->insert_status (status);

  _client_id_map.insert (uuid, name);
}

void SlateWindow::user_del (DnDClient* client, Uuid uuid)
{
  QMap<Uuid, QString>::iterator rm = _client_id_map.find (uuid);

  QString status = *rm;
  status += " disconnected";

  _chat_widget->insert_status (status);

  _client_id_map.erase (rm);
}

void SlateWindow::disconnect_client ()
{
  _connect_action->setEnabled (true);
  _disconnect_action->setEnabled (false);

  disconnect (_client, 0, 0, 0);

  if (_server)
    delete _server;

  _status_label->setText ("Disconnected");
}
