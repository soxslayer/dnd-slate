#ifndef __SLATE_WINDOW__
#define __SLATE_WINDOW__

#include <QMainWindow>
#include <QString>
#include <QMap>

#include "uuid.h"

class QAction;
class QLabel;
class DnDServer;
class DnDClient;
class ChatWidget;

class SlateWindow : public QMainWindow
{
  Q_OBJECT;

public:
  SlateWindow (const QString& bin_path);
  ~SlateWindow () { }

private slots:
  void open_triggered (bool checked);
  void quit_triggered (bool checked);
  void connect_triggered (bool checked);
  void disconnect_triggered (bool checked);
  void server_connected ();
  void server_disconnected ();
  void user_add_resp (DnDClient* client, Uuid uuid, const QString& name);
  void user_del (DnDClient* client, Uuid);

private:
  QString _bin_path;
  QAction* _connect_action;
  QAction* _disconnect_action;
  QLabel* _status_label;
  ChatWidget* _chat_widget;
  DnDServer* _server;
  DnDClient* _client;
  QMap<Uuid, QString> _client_id_map;
  QString _name;

  void disconnect_client ();
};

#endif /* __SLATE_WINDOW__ */
