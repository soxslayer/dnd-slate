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

#ifndef __DND_CLIENT__
#define __DND_CLIENT__

#include <QTcpSocket>
#include <QString>
#include <QQueue>

#include "uuid.h"

class QObject;
class Buffer;
struct DnDMessageHeader;

class DnDClient : public QTcpSocket
{
  Q_OBJECT

public:
  DnDClient (QObject* parent = 0);
  DnDClient (const QString& host, quint16 port, QObject* parent = 0);

signals:
  void disconnected (DnDClient* client);
  void user_add_req (DnDClient* client, const QString& name);
  void user_add_resp (DnDClient* client, Uuid uuid, const QString& name);
  void user_del (DnDClient* client, Uuid uuid);
  void chat_message (DnDClient* client, Uuid src, Uuid dst,
                     const QString& message, int flags);

public slots:
  void user_add_req (const QString& name);
  void user_add_resp (Uuid uuid, const QString& name);
  void user_del (Uuid uuid);
  void chat_message (Uuid src, Uuid dst, const QString& msg, int flags);

private slots:
  void disconnected ();
  void ready_read ();
  void bytes_written (qint64 bytes);

private:
  Buffer* _sync_buffer;
  Buffer* _cur_msg_buff;
  QQueue<Buffer*> _send_queue;

  void connect_signals ();
  quint64 parse_packet (const void* buff, quint64 size);
  void handle_message (const DnDMessageHeader* header, quint64 size);
  void send_message (void* msg, quint64 size);
};

#endif /* __DND_CLIENT__ */
