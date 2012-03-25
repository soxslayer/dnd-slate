#include <cstddef>
#include <cstring>

#include <QObject>

#include "dnd_client.h"
#include "dnd_messages.h"
#include "net_buffer_pool.h"
#include "buffer.h"

#pragma pack(1)
  struct NetworkHeader
  {
    quint32 sync;
    quint16 size;
  };
#pragma pack()

DnDClient::DnDClient (QObject* parent)
  : QTcpSocket (parent),
    _cur_msg_buff (0)
{
  connect_signals ();

  _sync_buffer = NET_BUFFER_POOL.alloc (sizeof (NetworkHeader));
  _sync_buffer->setParent (this);
}

DnDClient::DnDClient (const QString& host, quint16 port, QObject* parent)
  : QTcpSocket (parent),
    _cur_msg_buff (0)
{
  connect_signals ();

  connectToHost (host, port);

  _sync_buffer = NET_BUFFER_POOL.alloc (sizeof (NetworkHeader));
  _sync_buffer->setParent (this);
}

void DnDClient::user_add_req (const QString& name)
{
  quint64 msg_size = sizeof (DnDUserAddReq) - 1 + name.size ();
  DnDUserAddReq* msg = (DnDUserAddReq*)new char[msg_size];

  msg->header.type = DND_USER_ADD_REQ;
  memcpy (&msg->name, name.data (), name.size ());

  send_message (msg, msg_size);

  delete [] msg;
}

void DnDClient::user_add_resp (Uuid uuid, const QString& name)
{
  quint64 msg_size = sizeof (DnDUserAddResp) - 1 + name.size ();
  DnDUserAddResp* msg = (DnDUserAddResp*)new char[msg_size];

  msg->header.type = DND_USER_ADD_RESP;
  msg->uuid = uuid;
  memcpy (&msg->name, name.data (), name.size ());

  send_message (msg, msg_size);

  delete [] msg;
}

void DnDClient::user_del (Uuid uuid)
{
  DnDUserDel msg;

  msg.header.type = DND_USER_DEL;
  msg.uuid = uuid;

  send_message (&msg, sizeof (msg));
}

void DnDClient::disconnected ()
{
  disconnected (this);
}

void DnDClient::ready_read ()
{
  quint64 nread = bytesAvailable ();
  quint64 nparsed = 0;
  char* data = new char[nread];

  read (data, nread);

  while ((nparsed += parse_packet (data + nparsed, nread - nparsed)) < nread);

  delete [] data;
}

void DnDClient::bytes_written (qint64 bytes)
{
  Buffer* buff = _send_queue.head ();

  while ((bytes -= buff->discard (bytes))) {
    if (!buff->get_available_size ()) {
      _send_queue.dequeue ();
      buff = _send_queue.head ();
    }
  }
}

void DnDClient::connect_signals ()
{
  connect (this, SIGNAL (readyRead ()), this, SLOT (ready_read ()));
  connect (this, SIGNAL (bytesWritten (qint64)),
           this, SLOT (bytes_written (qint64)));
  connect (this, SIGNAL (disconnected ()), this, SLOT (disconnected ()));
}

quint64 DnDClient::parse_packet (const void* data, quint64 size)
{
  quint64 nparsed = 0;

  if (!_cur_msg_buff) {
    nparsed += _sync_buffer->fill (data, size);
    size -= nparsed;
    data = (char*)data + nparsed;

    if (_sync_buffer->get_available_size ())
      return nparsed;

    NetworkHeader* hdr = (NetworkHeader*)_sync_buffer->get_data ();
    if (hdr->sync != SYNC_FIELD) {
      _sync_buffer->discard (1);
      return nparsed;
    }

    _cur_msg_buff = NET_BUFFER_POOL.alloc (hdr->size);
    _cur_msg_buff->setParent (this);
    _sync_buffer->clear ();
  }

  if (!size)
    return nparsed;

  nparsed += _cur_msg_buff->fill (data, size);

  if (!_cur_msg_buff->get_available_size ()) {
    handle_message ((DnDMessageHeader*)_cur_msg_buff->get_data ());
    NET_BUFFER_POOL.free (_cur_msg_buff);
    _cur_msg_buff = 0;
  }

  return nparsed;
}

void DnDClient::handle_message (const DnDMessageHeader* header)
{
  switch (header->type) {
    case DND_USER_ADD_REQ: {
      const DnDUserAddReq* msg = (DnDUserAddReq*)header;
      QString name ((const QChar*)msg->name,
                    sizeof (DnDUserAddReq) - offsetof (DnDUserAddReq, name));
      user_add_req (this, name);
      break;
    }

    case DND_USER_ADD_RESP: {
      const DnDUserAddResp* msg = (DnDUserAddResp*)header;
      QString name ((const QChar*)msg->name,
                    sizeof (DnDUserAddResp) - offsetof (DnDUserAddResp, name));
      user_add_resp (this, msg->uuid, name);
      break;
    }

    case DND_USER_DEL: {
      const DnDUserDel* msg = (DnDUserDel*)header;
      user_del (this, msg->uuid);
      break;
    }
  }
}

void DnDClient::send_message (void* msg, quint64 size)
{
  Buffer* msg_buff = NET_BUFFER_POOL.alloc (sizeof (NetworkHeader) + size);
  msg_buff->setParent (this);
  NetworkHeader hdr;

  hdr.sync = SYNC_FIELD;
  hdr.size = size;

  msg_buff->fill (&hdr, sizeof (hdr));
  msg_buff->fill (msg, size);

  _send_queue.enqueue (msg_buff);

  write ((char*)msg_buff->get_data (), msg_buff->get_filled_size ());
}
