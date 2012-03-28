#ifndef __SLATE_WINDOW__
#define __SLATE_WINDOW__

#include <QMainWindow>
#include <QString>

#include "uuid.h"

class QAction;
class QLabel;
class DnDServer;
class DnDClient;
class ChatWidget;
class PlayerList;

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
  void send_message (const QString& who, const QString& message);
  void user_add_resp (DnDClient* client, Uuid uuid, const QString& name);
  void user_del (DnDClient* client, Uuid);
  void chat_message (DnDClient* client, Uuid src, Uuid dst,
                     const QString& message, int flags);

private:
  QString _bin_path;
  QAction* _connect_action;
  QAction* _disconnect_action;
  QLabel* _status_label;
  ChatWidget* _chat_widget;
  PlayerList* _player_list;
  DnDServer* _server;
  DnDClient* _client;
  QString _name;

  void disconnect_client ();
};

#endif /* __SLATE_WINDOW__ */
