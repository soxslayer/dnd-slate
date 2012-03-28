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
