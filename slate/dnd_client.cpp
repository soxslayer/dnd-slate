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

#include <cstddef>
#include <cstring>

#include <QObject>
#include <QByteArray>

#include "dnd_client.h"

#include "dnd_messages.h"
#include "net_buffer_pool.h"
#include "buffer.h"
#include "image_id.h"

#ifdef NETWORK_DEBUG
# include <QDebug>

# include "sha1.h"
# include "sha_util.h"
#endif

#pragma pack(1)

struct NetworkHeader
{
  quint32 sync;
  quint16 size;
};

#pragma pack()

DnDClient::DnDClient (QObject* parent)
  : QTcpSocket (parent),
    Serializable (true),
    _cur_msg_buff (nullptr),
    _cur_send_buff (nullptr)
{
  connect_signals ();

  _sync_buffer = NET_BUFFER_POOL.alloc (sizeof (NetworkHeader));
}

DnDClient::DnDClient (const QString& host, quint16 port, QObject* parent)
  : QTcpSocket (parent),
    Serializable (true),
    _cur_msg_buff (nullptr),
    _cur_send_buff (nullptr)
{
  connect_signals ();

  connectToHost (host, port);

  _sync_buffer = NET_BUFFER_POOL.alloc (sizeof (NetworkHeader));
}

DnDClient::~DnDClient ()
{
  NET_BUFFER_POOL.free (_sync_buffer);

  if (_cur_msg_buff)
    NET_BUFFER_POOL.free (_cur_msg_buff);
}

void DnDClient::send_image (const QByteArray& image)
{
  image_begin (image.size ());

  for (int i = 0, s = 0; i < image.size ();
       i += DND_IMAGE_MAX_CHUNK_SIZE, ++s) {
    QByteArray chunk = image.mid (i, DND_IMAGE_MAX_CHUNK_SIZE);
    image_data (s, chunk);
  }
}

void DnDClient::comm_proto_req ()
{
  DnDCommProtoReq msg;

  msg.header.type = DND_COMM_PROTO_REQ;

  send_message (&msg, sizeof (msg), Priority0);
}

void DnDClient::comm_proto_resp (quint16 major, quint16 minor)
{
  DnDCommProtoResp msg;

  msg.header.type = DND_COMM_PROTO_RESP;
  msg.major = major;
  msg.minor = minor;

  send_message (&msg, sizeof (msg), Priority0);
}

void DnDClient::server_message (const QString& message, int flags)
{
  quint64 msg_size = sizeof (DnDServerMessage) - 1 + message.size ();
  DnDServerMessage* msg = (DnDServerMessage*)new char[msg_size];

  msg->header.type = DND_SERVER_MESSAGE;
  msg->flags = flags;
  QByteArray ascii_message = message.toAscii ();
  memcpy (msg->msg, ascii_message.data (), ascii_message.size ());

  send_message (msg, msg_size, Priority0);

  delete [] msg;
}

void DnDClient::user_add_req (const QString& name)
{
  quint64 msg_size = sizeof (DnDUserAddReq) - 1 + name.size ();
  DnDUserAddReq* msg = (DnDUserAddReq*)new char[msg_size];

  msg->header.type = DND_USER_ADD_REQ;
  QByteArray ascii_name = name.toAscii ();
  memcpy (msg->name, ascii_name.data (), ascii_name.size ());

  send_message (msg, msg_size, Priority0);

  delete [] msg;
}

void DnDClient::user_add_resp (Uuid uuid, const QString& name)
{
  quint64 msg_size = sizeof (DnDUserAddResp) - 1 + name.size ();
  DnDUserAddResp* msg = (DnDUserAddResp*)new char[msg_size];

  msg->header.type = DND_USER_ADD_RESP;
  msg->uuid = uuid;
  QByteArray ascii_name = name.toAscii ();
  memcpy (msg->name, ascii_name.data (), ascii_name.size ());

  send_message (msg, msg_size, Priority0);

  delete [] msg;
}

void DnDClient::user_del (Uuid uuid)
{
  DnDUserDel msg;

  msg.header.type = DND_USER_DEL;
  msg.uuid = uuid;

  send_message (&msg, sizeof (msg), Priority0);
}

void DnDClient::chat_message (Uuid src_uuid, Uuid dst_uuid,
                              const QString& message, int flags)
{
  quint64 msg_size = sizeof (DnDChatMessage) - 1 + message.size ();
  DnDChatMessage* msg = (DnDChatMessage*)new char[msg_size];

  msg->header.type = DND_CHAT_MESSAGE;
  msg->src_uuid = src_uuid;
  msg->dst_uuid = dst_uuid;
  QByteArray ascii_msg = message.toAscii ();
  memcpy (msg->message, ascii_msg.data (), ascii_msg.size ());
  msg->flags = flags;

  send_message (msg, msg_size, Priority0);

  delete [] msg;
}

void DnDClient::load_map (quint16 w, quint16 h, const ImageId& image_id)
{
  DnDLoadMap msg;

  msg.header.type = DND_LOAD_MAP;
  msg.w = w;
  msg.h = h;
  memcpy (msg.image_id, image_id.data ().constData (), sizeof (msg.image_id));

  send_message (&msg, sizeof (msg), Priority0);
}

void DnDClient::request_image (const ImageId& image_id)
{
  DnDRequestImage msg;

  msg.header.type = DND_REQUEST_IMAGE;
  memcpy (msg.image_id, image_id.data (), sizeof (msg.image_id));

  send_message (&msg, sizeof (msg), Priority0);
}

void DnDClient::image_begin (quint64 total_size)
{
  DnDImageBegin msg;

  msg.header.type = DND_IMAGE_BEGIN;
  msg.total_size = total_size;

  send_message (&msg, sizeof (msg), Priority0);
}

void DnDClient::image_data (quint32 sequence, const QByteArray& data)
{
  quint64 msg_size = sizeof (DnDImageData) - 1 + data.size ();
  DnDImageData* msg = (DnDImageData*)new char[msg_size];

  msg->header.type = DND_IMAGE_DATA;
  msg->sequence = sequence;
  memcpy (msg->data, data.constData (), data.size ());

  send_message (msg, msg_size, Priority1);

  delete [] msg;
}

void DnDClient::add_tile (Uuid tile_uuid, quint8 type, quint16 x, quint16 y,
                          quint16 w, quint16 h, const QString& text)
{
  quint64 msg_size = sizeof (DnDAddTile) -1 + text.size ();
  DnDAddTile* msg = (DnDAddTile*)new char[msg_size];

  msg->header.type = DND_ADD_TILE;
  msg->tile_uuid = tile_uuid;
  msg->type = type;
  msg->x = x;
  msg->y = y;
  msg->w = w;
  msg->h = h;
  QByteArray ascii_text = text.toAscii ();
  memcpy (msg->text, ascii_text.data (), ascii_text.size ());

  send_message (msg, msg_size, Priority0);

  delete [] msg;
}

void DnDClient::move_tile (Uuid tile_uuid, quint16 x, quint16 y)
{
  DnDMoveTile msg;

  msg.header.type = DND_MOVE_TILE;
  msg.tile_uuid = tile_uuid;
  msg.x = x;
  msg.y = y;

  send_message (&msg, sizeof (msg), Priority0);
}

void DnDClient::delete_tile (Uuid tile_uuid)
{
  DnDDeleteTile msg;

  msg.header.type = DND_DELETE_TILE;
  msg.tile_uuid = tile_uuid;

  send_message (&msg, sizeof (msg), Priority0);
}

void DnDClient::ping_pong ()
{
  DnDPingPong msg;

  msg.header.type = DND_PING_PONG;

  send_message (&msg, sizeof (msg), Priority0);
}

void DnDClient::ping_pong_record (Uuid player_uuid, quint32 delay)
{
  DnDPingPongRecord msg;

  msg.header.type = DND_PING_PONG_RECORD;
  msg.player_uuid = player_uuid;
  msg.delay = delay;

  send_message (&msg, sizeof (msg), Priority0);
}

void DnDClient::image_query (const ImageId& image_id)
{
  DnDImageQuery msg;

  msg.header.type = DND_IMAGE_QUERY;
  memcpy (msg.image_id, image_id.data (), sizeof (msg.image_id));

  send_message (&msg, sizeof (msg), Priority0);
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
  _cur_send_buff->discard (bytes);

  if (!_cur_send_buff->get_filled_size ()) {
    NET_BUFFER_POOL.free (_cur_send_buff);
    _cur_send_buff = nullptr;

    for (int i = 0; i <= PriorityMax; ++i) {
      if (_send_queues[i].size ()) {
        _cur_send_buff = _send_queues[i].head ();
        _send_queues[i].dequeue ();
        write_data (_cur_send_buff->get_data (),
                    _cur_send_buff->get_filled_size ());
      }
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
    _sync_buffer->clear ();
  }

  if (!size)
    return nparsed;

  nparsed += _cur_msg_buff->fill (data, size);

  if (!_cur_msg_buff->get_available_size ()) {
#ifdef NETWORK_DEBUG
    SHA1 sig;

    sig.Input ((const char*)_cur_msg_buff->get_data (),
               _cur_msg_buff->get_size ());

    qDebug () << "Received packet, sig:" << sha1_to_string (sig);
#endif

    handle_message ((DnDMessageHeader*)_cur_msg_buff->get_data (),
                    _cur_msg_buff->get_size ());
    NET_BUFFER_POOL.free (_cur_msg_buff);
    _cur_msg_buff = nullptr;
  }

  return nparsed;
}

void DnDClient::handle_message (const DnDMessageHeader* header, quint64 size)
{
  switch (header->type) {
    case DND_COMM_PROTO_REQ:
      comm_proto_req (this);
      break;

    case DND_COMM_PROTO_RESP: {
      const DnDCommProtoResp* msg = (DnDCommProtoResp*)header;
      comm_proto_resp (this, msg->major, msg->minor);
      break;
    }

    case DND_SERVER_MESSAGE: {
      const DnDServerMessage* msg = (DnDServerMessage*)header;
      QString message = QString::fromAscii (msg->msg,
        size - offsetof (DnDServerMessage, msg));
      server_message (this, message, msg->flags);
      break;
    }

    case DND_USER_ADD_REQ: {
      const DnDUserAddReq* msg = (DnDUserAddReq*)header;
      QString name = QString::fromAscii (msg->name,
        size - offsetof (DnDUserAddReq, name));
      user_add_req (this, name);
      break;
    }

    case DND_USER_ADD_RESP: {
      const DnDUserAddResp* msg = (DnDUserAddResp*)header;
      QString name = QString::fromAscii (msg->name,
        size - offsetof (DnDUserAddResp, name));
      user_add_resp (this, msg->uuid, name);
      break;
    }

    case DND_USER_DEL: {
      const DnDUserDel* msg = (DnDUserDel*)header;
      user_del (this, msg->uuid);
      break;
    }

    case DND_CHAT_MESSAGE: {
      const DnDChatMessage* msg = (DnDChatMessage*)header;
      QString message = QString::fromAscii (msg->message,
        size - offsetof (DnDChatMessage, message));
      chat_message (this, msg->src_uuid, msg->dst_uuid, message, msg->flags);
      break;
    }

    case DND_LOAD_MAP: {
      const DnDLoadMap* msg = (DnDLoadMap*)header;
      ImageId image_id (msg->image_id, sizeof (msg->image_id));
      load_map (this, msg->w, msg->h, image_id);
      break;
    }

    case DND_REQUEST_IMAGE: {
      const DnDRequestImage* msg = (DnDRequestImage*)header;
      ImageId image_id (msg->image_id, sizeof (msg->image_id));
      request_image (this, image_id);
      break;
    }

    case DND_IMAGE_BEGIN: {
      const DnDImageBegin* msg = (DnDImageBegin*)header;
      image_begin (this, msg->total_size);
      break;
    }

    case DND_IMAGE_DATA: {
      const DnDImageData* msg = (DnDImageData*)header;
      quint64 data_size = size - offsetof (DnDImageData, data);
      QByteArray data (msg->data, data_size);
      image_data (this, msg->sequence, data);
      break;
    }

    case DND_ADD_TILE: {
      const DnDAddTile* msg = (DnDAddTile*)header;
      QString text = QString::fromAscii (msg->text,
        size - offsetof (DnDAddTile, text));
      add_tile (this, msg->tile_uuid, msg->type, msg->x, msg->y, msg->w,
                msg->h, text);
      break;
    }

    case DND_MOVE_TILE: {
      const DnDMoveTile* msg = (DnDMoveTile*)header;
      move_tile (this, msg->tile_uuid, msg->x, msg->y);
      break;
    }

    case DND_DELETE_TILE: {
      const DnDDeleteTile* msg = (DnDDeleteTile*)header;
      delete_tile (this, msg->tile_uuid);
      break;
    }

    case DND_PING_PONG:
      ping_pong (this);
      break;

    case DND_PING_PONG_RECORD: {
      const DnDPingPongRecord* msg = (DnDPingPongRecord*)header;
      ping_pong_record (this, msg->player_uuid, msg->delay);
      break;
    }

    case DND_IMAGE_QUERY: {
      const DnDImageQuery* msg = (DnDImageQuery*)header;
      ImageId image_id (msg->image_id, sizeof (msg->image_id));
      image_query (this, image_id);
      break;
    }
  }
}

void DnDClient::send_message (void* msg, quint64 size,
                              MessagePriority priority)
{
  Buffer* msg_buff = NET_BUFFER_POOL.alloc (sizeof (NetworkHeader) + size);
  NetworkHeader hdr;

  hdr.sync = SYNC_FIELD;
  hdr.size = size;

  msg_buff->fill (&hdr, sizeof (hdr));
  msg_buff->fill (msg, size);

  WLOCK ();

  if (!_cur_send_buff) {
    write_data (msg_buff->get_data (), msg_buff->get_filled_size ());
    _cur_send_buff = msg_buff;
  }
  else
    _send_queues[priority].enqueue (msg_buff);
}

void DnDClient::write_data (const void* buff, quint64 size)
{
#ifdef NETWORK_DEBUG
  SHA1 sig;

  sig.Input ((const char*)buff, size);

  qDebug () << "Sending packet, sig:" << sha1_to_string (sig);
#endif

  write ((const char*)buff, size);
}
