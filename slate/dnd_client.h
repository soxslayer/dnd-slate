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

#ifndef __DND_CLIENT__
#define __DND_CLIENT__

#include <QTcpSocket>
#include <QString>
#include <QQueue>

#include "uuid.h"
#include "serializable.h"

class QObject;
class QByteArray;
class Buffer;
class ImageId;
struct DnDMessageHeader;

class DnDClient : public QTcpSocket, public Serializable
{
  Q_OBJECT

public:
  DnDClient (QObject* parent = nullptr);
  DnDClient (const QString& host, quint16 port, QObject* parent = nullptr);
  ~DnDClient ();

  void send_image (const QByteArray& image);

signals:
  void disconnected (DnDClient* client);
  void comm_proto_req (DnDClient* client);
  void comm_proto_resp (DnDClient* client, quint16 major, quint16 minor);
  void server_message (DnDClient* client, const QString& message, int flags);
  void user_add_req (DnDClient* client, const QString& name);
  void user_add_resp (DnDClient* client, Uuid uuid, const QString& name);
  void user_del (DnDClient* client, Uuid uuid);
  void chat_message (DnDClient* client, Uuid src_uuid, Uuid dst_uuid,
                     const QString& message, int flags);
  void load_map (DnDClient* client, quint16 w, quint16 h,
                 const ImageId& image_id);
  void request_image (DnDClient* client, const ImageId& image_id);
  void image_begin (DnDClient* client, quint64 total_size);
  void image_data (DnDClient* client, quint32 sequence,
                   const QByteArray& data);
  void add_tile (DnDClient* client, Uuid tile_uuid, quint8 type, quint16 x,
                 quint16 y, quint16 w, quint16 h, const QString& text);
  void move_tile (DnDClient* client, Uuid tile_uuid, quint16 x, quint16 y);
  void delete_tile (DnDClient* client, Uuid tile_uuid);
  void ping_pong (DnDClient* client);
  void ping_pong_record (DnDClient* client, Uuid player_uuid, quint32 delay);
  void image_query (DnDClient* client, const ImageId& image_id);

public slots:
  void comm_proto_req ();
  void comm_proto_resp (quint16 major, quint16 minor);
  void server_message (const QString& message, int flags);
  void user_add_req (const QString& name);
  void user_add_resp (Uuid uuid, const QString& name);
  void user_del (Uuid uuid);
  void chat_message (Uuid src_uuid, Uuid dst_uuid, const QString& msg,
                     int flags);
  void load_map (quint16 w, quint16 h, const ImageId& image_id);
  void request_image (const ImageId& image_id);
  void image_begin (quint64 total_size);
  void image_data (quint32 sequence, const QByteArray& data);
  void add_tile (Uuid tile_uuid, quint8 type, quint16 x, quint16 y, quint16 w,
                 quint16 h, const QString& text);
  void move_tile (Uuid tile_uuid, quint16 x, quint16 y);
  void delete_tile (Uuid tile_uuid);
  void ping_pong ();
  void ping_pong_record (Uuid player_uuid, quint32 delay);
  void image_query (const ImageId& image_id);

private slots:
  void disconnected ();
  void ready_read ();
  void bytes_written (qint64 bytes);

private:
  typedef enum
  {
    Priority0,
    Priority1,
    PriorityMax = Priority1
  } MessagePriority;

  Buffer* _sync_buffer;
  Buffer* _cur_msg_buff;
  QQueue<Buffer*> _send_queues[PriorityMax + 1];
  Buffer* _cur_send_buff;

  void connect_signals ();
  quint64 parse_packet (const void* buff, quint64 size);
  void handle_message (const DnDMessageHeader* header, quint64 size);
  void send_message (void* msg, quint64 size, MessagePriority priority);
  void write_data (const void* buff, quint64 size);
};

#endif /* __DND_CLIENT__ */
