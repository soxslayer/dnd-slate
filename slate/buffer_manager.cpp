#include "buffer_manager.h"
#include "buffer.h"

Buffer* BufferManager::alloc (quint64 size)
{
  QMultiHash<quint64, Buffer*>::iterator buff_i = _free_list.find (size);
  Buffer* buff = 0;

  if (buff_i == _free_list.end ())
    buff = new Buffer (size);
  else {
    buff = *buff_i;
    _free_list.erase (buff_i);
  }

  /* Set the parent to 0 so the buffer isn't deleted while it is still
   * allocated. */
  buff->setParent (0);
  buff->clear ();

  return buff;
}

void BufferManager::free (Buffer* buff)
{
  _free_list.insert (buff->get_size (), buff);

  /* Set the parent to the manager so it's automatically deleted */
  buff->setParent (this);
}
