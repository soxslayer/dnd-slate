#ifndef __BUFFER_MANAGER__
#define __BUFFER_MANAGER__

#include <QObject>
#include <QMultiHash>

class Buffer;

class BufferManager : public QObject
{
public:
  BufferManager (QObject* parent = 0) : QObject (parent) { }

  Buffer* alloc (quint64 size);
  void free (Buffer* buff);

private:
  QMultiHash<quint64, Buffer*> _free_list;
};

#endif /* __BUFFER_MANAGER__ */
