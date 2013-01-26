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

#ifndef __SERIALIZABLE__
#define __SERIALIZABLE__

class QReadWriteLock;

class ReadLock
{
public:
  ReadLock ();
  ReadLock (QReadWriteLock* lock);
  /* Moves the held read lock to the instance being created */
  ReadLock (ReadLock&& lock);
  ~ReadLock ();

private:
  QReadWriteLock* _lock;

  ReadLock (const ReadLock&) { }
  const ReadLock& operator= (const ReadLock&) { return *this; }
};

class WriteLock
{
public:
  WriteLock ();
  WriteLock (QReadWriteLock* lock);
  /* Moves the hels write lock to the instances being created */
  WriteLock (WriteLock&& lock);
  ~WriteLock ();

private:
  QReadWriteLock* _lock;

  WriteLock (const WriteLock&) { }
  const WriteLock& operator= (const WriteLock&) { return *this; }
};

#define RLOCK() auto l = read_lock ()
#define WLOCK() auto l = write_lock ()

class Serializable
{
public:
  Serializable (bool serialized = false);
  ~Serializable ();

  void serialize ();
  ReadLock read_lock () const;
  WriteLock write_lock ();

private:
  QReadWriteLock* _lock;
};

#endif /* __SERIALIZABLE__ */
