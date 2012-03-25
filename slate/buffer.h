#ifndef __BUFFER__
#define __BUFFER__

#include <QObject>

class Buffer : public QObject
{
public:
  Buffer (quint64 size, QObject* parent = 0);
  virtual ~Buffer ();

  quint64 get_size () const { return _size; }
  quint64 get_available_size () const { return _size - _offset; }
  quint64 get_filled_size () const { return _offset; }
  void* get_data () { return _data; }
  quint64 fill (const void* src, quint64 size);
  quint64 fill (quint64 size);
  quint64 empty (void* dst, quint64 size);
  quint64 discard (quint64 size);
  void clear ();

private:
  char* _data;
  quint64 _size;
  quint64 _offset;
};

#endif /* __BUFFER__ */
