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

#include <QHostAddress>
#include <QDebug>

#include "dnd_server.h"
#include "dnd_client.h"
#include "dnd_messages.h"

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
  connect (client,
    SIGNAL (chat_message (DnDClient*, Uuid, Uuid, const QString&, int)), this,
    SLOT (client_chat_message (DnDClient*, Uuid, Uuid, const QString&, int)));
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
    QString dm_prefix = "[DM] ";
    c_name.prepend (dm_prefix);
  }

  c_name.replace (' ', '_');

  client->user_add_resp (uuid, c_name);

  QMap<Uuid, ClientId*>::iterator beg = _client_map.begin ();
  QMap<Uuid, ClientId*>::iterator end = _client_map.end ();

  for (; beg != end; ++beg) {
    (*beg)->client->user_add_resp (uuid, c_name);
    client->user_add_resp (beg.key (), (*beg)->name);
  }

  _client_map.insert (uuid, new ClientId (c_name, client));
}

void DnDServer::client_chat_message (DnDClient* client, Uuid src, Uuid dst,
                                     const QString& message, int flags)
{
  QMap<Uuid, ClientId*>::iterator beg = _client_map.begin ();
  QMap<Uuid, ClientId*>::iterator end = _client_map.end ();

  for (; beg != end; ++beg) {
    if (flags & CHAT_BROADCAST || src == beg.key () || dst == beg.key ())
      (*beg)->client->chat_message (src, dst, message, flags);
  }
}
