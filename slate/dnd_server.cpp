#include <QHostAddress>
#include <QDebug>

#include "dnd_server.h"
#include "dnd_client.h"

DnDServer::DnDServer (quint16 port)
{
  listen (QHostAddress::Any, port);
}

DnDServer::~DnDServer ()
{
  QMap<Uuid, ClientId*>::iterator beg = _client_map.begin ();
  QMap<Uuid, ClientId*>::iterator end = _client_map.end ();

  for (; beg != end; ++beg)
    delete *beg;
}

void DnDServer::incomingConnection (int socketDescriptor)
{
  DnDClient* client = new DnDClient (this);
  client->setSocketDescriptor (socketDescriptor);

  connect (client, SIGNAL (user_add_req (DnDClient*, const QString&)),
           this, SLOT (client_user_add_req (DnDClient*, const QString&)));
  connect (client, SIGNAL (disconnected (DnDClient*)),
           this, SLOT (client_disconnected (DnDClient*)));
}

void DnDServer::client_disconnected (DnDClient* client)
{
  QMap<Uuid, ClientId*>::iterator beg = _client_map.begin ();
  QMap<Uuid, ClientId*>::iterator end = _client_map.end ();
  Uuid rm_uuid;
  QMap<Uuid, ClientId*>::iterator rm;

  for (; beg != end; ++beg) {
    if (client == (*beg)->client) {
      rm = beg;
      rm_uuid = beg.key ();
      continue;
    }
  }

  _client_map.erase (rm);

  beg = _client_map.begin ();

  for (; beg != end; ++beg)
    (*beg)->client->user_del (rm_uuid);
}

void DnDServer::client_user_add_req (DnDClient* client, const QString& name)
{
  Uuid uuid = _uuid_manager.get_uuid ();
  QString c_name = name;

  if (_client_map.isEmpty ()) {
    _dm_uuid = uuid;
    c_name.append ("[DM] ");
  }

  client->user_add_resp (uuid, name);

  QMap<Uuid, ClientId*>::iterator beg = _client_map.begin ();
  QMap<Uuid, ClientId*>::iterator end = _client_map.end ();

  for (; beg != end; ++beg) {
    (*beg)->client->user_add_resp (uuid, name);
    client->user_add_resp (beg.key (), (*beg)->name);
  }

  _client_map.insert (uuid, new ClientId (name, client));
}
