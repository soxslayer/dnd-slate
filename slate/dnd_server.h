#ifndef __DND_SERVER__
#define __DND_SERVER__

#include <QTcpServer>
#include <QMap>

#include "uuid.h"

class DnDClient;

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
  void client_user_add_req (DnDClient* client, const QString& name);

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
  QMap<Uuid, ClientId*> _client_map;
};

#endif /* __DND_SERVER__ */
