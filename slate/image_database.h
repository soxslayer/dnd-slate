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

#ifndef __IMAGE_DATABASE__
#define __IMAGE_DATABASE__

#include <QHash>
#include <QString>

class QByteArray;

#include "sha_util.h"
#include "image_id.h"

class ImageDatabase
{
public:
  static void init ();
  static ImageDatabase& get_instance ();

  ImageId add (const QString& filename);
  ImageId add (const QByteArray& image);
  QString get_path (const ImageId& sha);
  ImageId get_sha (const QString& filename);
  bool has_entry (const ImageId& sha) { return _hash.count (sha) != 0; }
  void set_autoflush (bool auto_flush = true) { _af = auto_flush; }
  void flush () const;

private:
  static ImageDatabase* _instance;
  static QString _image_db_dir;
  static QString _image_db_file;

  struct DBEntry
  {
    DBEntry (const QString& path) : path (path), dirty (true) { }

    QString path;
    bool dirty;
  };

  QHash<ImageId, DBEntry*> _hash;
  QHash<QString, ImageId> _reverse_hash;
  bool _af;

  ImageDatabase ();
  ImageDatabase (const ImageDatabase&) { }
  const ImageDatabase& operator= (const ImageDatabase&) { return *this; }
  ~ImageDatabase ();

  void read ();
  QString clean_up_filename (const QString& filename);
  ImageId calculate_sha (const QString& filename);
  void add_entry (const ImageId& id, const QString& filename,
                  bool dirty = true);
};

#endif /* __IMAGE_DATABASE__ */
