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
