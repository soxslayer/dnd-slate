#include <cstring>

#include "buffer.h"

using namespace std;

Buffer::Buffer (quint64 size, QObject* parent)
  : QObject (parent)
{
  _size = size;
  _offset = 0;

  _data = new char[size];
}

Buffer::~Buffer ()
{
  delete [] _data;
}

quint64 Buffer::fill (const void* src, quint64 size)
{
  if (!src)
    return 0;

  quint64 ncopy = size > (_size - _offset) ? (_size - _offset) : size;

  if (!ncopy)
    return 0;

  memcpy (_data + _offset, src, ncopy);

  _offset += ncopy;

  return ncopy;
}

quint64 Buffer::fill (quint64 size)
{
  quint64 ncopy = size > (_size - _offset) ? (_size - _offset) : size;
  _offset += ncopy;
  return ncopy;
}

quint64 Buffer::empty (void* dst, quint64 size)
{
  if (!dst)
    return 0;

  quint64 ncopy = size > (_size - _offset) ? (_size - _offset) : size;

  if (!ncopy)
    return 0;

  memcpy (dst, _data, ncopy);
  discard (ncopy);

  return ncopy;
}

quint64 Buffer::discard (quint64 size)
{
  quint64 nmove = size > _size ? _size : size;

  if (!nmove)
    return 0;

  memmove (_data, _data + nmove, _size - nmove);

  _offset -= nmove;

  return nmove;
}

void Buffer::clear ()
{
  _offset = 0;
}
