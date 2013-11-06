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

#ifndef __IMAGE_ID__
#define __IMAGE_ID__

#include <QString>
#include <QByteArray>

class ImageId
{
public:
  enum
  {
    ID_LEN = 40
  };

  ImageId ();
  ImageId (const char* id, int size = -1);
  ImageId (const ImageId& id);
  ImageId (const QString& id);

  const ImageId& operator= (const ImageId& id);
  const ImageId& operator= (const QString& id);

  QByteArray data () const { return _id.toAscii (); }
  bool is_valid () const { return !_id.isEmpty (); }

  operator QString& () { return _id; }
  operator const QString& () const { return _id; }
  bool operator== (const ImageId& id) const { return _id == id._id; }
  bool operator== (const QString& id) const { return _id == id; }
  bool operator!= (const ImageId& id) const { return !(*this == id); }
  bool operator!= (const QString& id) const { return !(*this == id); }

private:
  QString _id;
};

#endif /* __IMAGE_ID__ */
