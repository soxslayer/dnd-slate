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

#include <QReadWriteLock>

#include "serializable.h"

ReadLock::ReadLock ()
  : _lock (nullptr)
{
}

ReadLock::ReadLock (QReadWriteLock* lock)
  : _lock (lock)
{
  if (_lock)
    _lock->lockForRead ();
}

ReadLock::ReadLock (ReadLock&& lock)
  : _lock (lock._lock)
{
  lock._lock = nullptr;
}

ReadLock::~ReadLock ()
{
  if (_lock)
    _lock->unlock ();
}



WriteLock::WriteLock ()
  : _lock (0)
{
}

WriteLock::WriteLock (QReadWriteLock* lock)
  : _lock (lock)
{
  if (_lock)
    _lock->lockForWrite ();
}

WriteLock::WriteLock (WriteLock&& lock)
  : _lock (lock._lock)
{
  lock._lock = nullptr;
}

WriteLock::~WriteLock ()
{
  if (_lock)
    _lock->unlock ();
}



Serializable::Serializable (bool serialized)
  : _lock (nullptr)
{
  if (serialized)
    serialize ();
}

Serializable::~Serializable ()
{
  if (_lock)
    delete _lock;
}

void Serializable::serialize ()
{
  if (_lock)
    return;

  _lock = new QReadWriteLock (QReadWriteLock::Recursive);
}

ReadLock Serializable::read_lock () const
{
  if (!_lock)
    return ReadLock ();

  return ReadLock (_lock);
}

WriteLock Serializable::write_lock ()
{
  if (!_lock)
    return WriteLock ();

  return WriteLock (_lock);
}
