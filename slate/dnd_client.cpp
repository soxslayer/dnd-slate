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

#include <cstddef>
#include <cstring>

#include <QObject>

#include "dnd_client.h"
#include <QByteArray>

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

void DnDClient::comm_proto_req ()
{
  DnDCommProtoReq msg;

  msg.header.type = DND_COMM_PROTO_REQ;

  send_message (&msg, sizeof (msg));
}

void DnDClient::comm_proto_resp (quint16 major, quint16 minor)
{
  DnDCommProtoResp msg;

  msg.header.type = DND_COMM_PROTO_RESP;
  msg.major = major;
  msg.minor = minor;

  send_message (&msg, sizeof (msg));
}

void DnDClient::server_message (const QString& message, int flags)
{
  quint64 msg_size = sizeof (DnDServerMessage) - 1 + message.size ();
  DnDServerMessage* msg = (DnDServerMessage*)new char[msg_size];

  msg->header.type = DND_SERVER_MESSAGE;
  msg->flags = flags;
  QByteArray ascii_message = message.toAscii ();
  memcpy (&msg->msg, ascii_message.data (), ascii_message.size ());

  send_message (msg, msg_size);

  delete [] msg;
}

void DnDClient::user_add_req (const QString& name)
{
  quint64 msg_size = sizeof (DnDUserAddReq) - 1 + name.size ();
  DnDUserAddReq* msg = (DnDUserAddReq*)new char[msg_size];

  msg->header.type = DND_USER_ADD_REQ;
  QByteArray ascii_name = name.toAscii ();
  memcpy (&msg->name, ascii_name.data (), ascii_name.size ());

  send_message (msg, msg_size);

  delete [] msg;
}

void DnDClient::user_add_resp (Uuid uuid, const QString& name)
{
  quint64 msg_size = sizeof (DnDUserAddResp) - 1 + name.size ();
  DnDUserAddResp* msg = (DnDUserAddResp*)new char[msg_size];

  msg->header.type = DND_USER_ADD_RESP;
  msg->uuid = uuid;
  QByteArray ascii_name = name.toAscii ();
  memcpy (&msg->name, ascii_name.data (), ascii_name.size ());

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

void DnDClient::chat_message (Uuid src, Uuid dst, const QString& message,
                              int flags)
{
  quint64 msg_size = sizeof (DnDChatMessage) - 1 + message.size ();
  DnDChatMessage* msg = (DnDChatMessage*)new char[msg_size];

  msg->header.type = DND_CHAT_MESSAGE;
  msg->src_uuid = src;
  msg->dst_uuid = dst;
  QByteArray ascii_msg = message.toAscii ();
  memcpy (&msg->message, ascii_msg.data (), ascii_msg.size ());
  msg->flags = flags;

  send_message (msg, msg_size);

  delete [] msg;
}

void DnDClient::load_image (const QString& file_name)
{
  quint64 msg_size = sizeof (DnDLoadImage) - 1 + file_name.size ();
  DnDLoadImage* msg = (DnDLoadImage*)new char[msg_size];

  msg->header.type = DND_LOAD_IMAGE;
  QByteArray ascii_file_name = file_name.toAscii ();
  memcpy (&msg->file_name, ascii_file_name.data (), ascii_file_name.size ());

  send_message (msg, msg_size);

  delete [] msg;
}

void DnDClient::image_begin (quint32 total_size, quint32 id)
{
  DnDImageBegin msg;

  msg.header.type = DND_IMAGE_BEGIN;
  msg.total_size = total_size;
  msg.id = id;

  send_message (&msg, sizeof (msg));
}

void DnDClient::image_data (quint32 id, quint32 sequence,
                            const uchar* data, quint64 size)
{
  quint64 msg_size = sizeof (DnDImageData) - 1 + size;
  DnDImageData* msg = (DnDImageData*)new char[msg_size];

  msg->header.type = DND_IMAGE_DATA;
  msg->id = id;
  msg->sequence = sequence;
  memcpy (&msg->data, data, size);

  send_message (msg, msg_size);

  delete [] msg;
}

void DnDClient::image_end (quint32 id)
{
  DnDImageEnd msg;

  msg.header.type = DND_IMAGE_END;
  msg.id = id;

  send_message (&msg, sizeof (msg));
}

void DnDClient::add_tile (Uuid uuid, quint8 type, quint16 x, quint16 y,
                          quint16 w, quint16 h, const QString& text)
{
  quint64 msg_size = sizeof (DnDAddTile) -1 + text.size ();
  DnDAddTile* msg = (DnDAddTile*)new char[msg_size];

  msg->header.type = DND_ADD_TILE;
  msg->uuid = uuid;
  msg->type = type;
  msg->x = x;
  msg->y = y;
  msg->w = w;
  msg->h = h;
  QByteArray ascii_text = text.toAscii ();
  memcpy (&msg->text, ascii_text.data (), ascii_text.size ());

  send_message (msg, msg_size);

  delete [] msg;
}

void DnDClient::move_tile (Uuid player_uuid, Uuid tile_uuid,
                           quint16 x, quint16 y)
{
  DnDMoveTile msg;

  msg.header.type = DND_MOVE_TILE;
  msg.player_uuid = player_uuid;
  msg.tile_uuid = tile_uuid;
  msg.x = x;
  msg.y = y;

  send_message (&msg, sizeof (msg));
}

void DnDClient::delete_tile (Uuid player_uuid, Uuid tile_uuid)
{
  DnDDeleteTile msg;

  msg.header.type = DND_DELETE_TILE;
  msg.player_uuid = player_uuid;
  msg.tile_uuid = tile_uuid;

  send_message (&msg, sizeof (msg));
}

void DnDClient::ping_pong (Uuid player_uuid)
{
  DnDPingPong msg;

  msg.header.type = DND_PING_PONG;
  msg.player_uuid = player_uuid;

  send_message (&msg, sizeof (msg));
}

void DnDClient::ping_pong_record (Uuid player_uuid, quint32 delay)
{
  DnDPingPongRecord msg;

  msg.header.type = DND_PING_PONG_RECORD;
  msg.player_uuid = player_uuid;
  msg.delay = delay;

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

  buff->discard (bytes);

  if (!buff->get_filled_size ()) {
    NET_BUFFER_POOL.free (buff);
    _send_queue.dequeue ();
  }

  if (_send_queue.size ()) {
    buff = _send_queue.head ();

    write ((char*)buff->get_data (), buff->get_filled_size ());
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
    handle_message ((DnDMessageHeader*)_cur_msg_buff->get_data (),
                    _cur_msg_buff->get_size ());
    NET_BUFFER_POOL.free (_cur_msg_buff);
    _cur_msg_buff = 0;
  }

  return nparsed;
}

void DnDClient::handle_message (const DnDMessageHeader* header, quint64 size)
{
  switch (header->type) {
    case DND_COMM_PROTO_REQ: {
      comm_proto_req (this);
      break;
    }

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

    case DND_LOAD_IMAGE: {
      const DnDLoadImage* msg = (DnDLoadImage*)header;
      QString file_name = QString::fromAscii (msg->file_name,
        size - offsetof (DnDLoadImage, file_name));
      load_image (this, file_name);
      break;
    }

    case DND_IMAGE_BEGIN: {
      const DnDImageBegin* msg = (DnDImageBegin*)header;
      image_begin (this, msg->total_size, msg->id);
      break;
    }

    case DND_IMAGE_DATA: {
      const DnDImageData* msg = (DnDImageData*)header;
      quint64 data_size = size - offsetof (DnDImageData, data);
      image_data (this, msg->id, msg->sequence, msg->data, data_size);
      break;
    }

    case DND_IMAGE_END: {
      const DnDImageEnd* msg = (DnDImageEnd*)header;
      image_end (this, msg->id);
      break;
    }

    case DND_ADD_TILE: {
      const DnDAddTile* msg = (DnDAddTile*)header;
      QString text = QString::fromAscii (msg->text,
        size - offsetof (DnDAddTile, text));
      add_tile (this, msg->uuid, msg->type, msg->x, msg->y, msg->w, msg->h,
                text);
      break;
    }

    case DND_MOVE_TILE: {
      const DnDMoveTile* msg = (DnDMoveTile*)header;
      move_tile (this, msg->player_uuid, msg->tile_uuid, msg->x, msg->y);
      break;
    }

    case DND_DELETE_TILE: {
      const DnDDeleteTile* msg = (DnDDeleteTile*)header;
      delete_tile (this, msg->player_uuid, msg->tile_uuid);
      break;
    }

    case DND_PING_PONG: {
      const DnDPingPong* msg = (DnDPingPong*)header;
      ping_pong (this, msg->player_uuid);
      break;
    }

    case DND_PING_PONG_RECORD: {
      const DnDPingPongRecord* msg = (DnDPingPongRecord*)header;
      ping_pong_record (this, msg->player_uuid, msg->delay);
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

  if (_send_queue.size () == 0)
    write ((char*)msg_buff->get_data (), msg_buff->get_filled_size ());

  _send_queue.enqueue (msg_buff);
}
