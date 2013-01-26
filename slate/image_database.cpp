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

#include <QFile>
#include <QTextStream>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <QByteArray>
#include <QDir>
#include <QFileInfo>
#include <QTemporaryFile>

#include "image_database.h"
#include "util.h"
#include "sha1.h"

ImageDatabase* ImageDatabase::_instance = 0;
QString ImageDatabase::_image_db_dir (".image_db");
QString ImageDatabase::_image_db_file (_image_db_dir + "/db");

void ImageDatabase::init ()
{
  ImageDatabase& db = get_instance ();

  db.read ();
}

ImageDatabase& ImageDatabase::get_instance ()
{
  if (!_instance)
    _instance = new ImageDatabase;

  return *_instance;
}

ImageId ImageDatabase::add (const QString& filename)
{
  QString fn = clean_up_filename (filename);

  if (_reverse_hash.count (fn) != 0) {
    return _reverse_hash[fn];
  }

  ImageId id = calculate_sha (fn);

  if (!id.is_valid ())
    return ImageId ();

  if (_hash.count (id) == 0) {
    add_entry (id, fn);

    if (_af)
      flush ();
  }

  return id;
}

ImageId ImageDatabase::add (const QByteArray& image)
{
  QString img_fn = _image_db_dir;

  img_fn.append ("/XXXXXXXXXX.png");

  QTemporaryFile img_f (img_fn);
  img_f.setAutoRemove (false);
  img_f.open ();

  if (img_f.isOpen ()) {
    img_f.write (image);
    img_f.close ();

    return add (img_f.fileName ());
  }

  return ImageId ();
}

QString ImageDatabase::get_path (const ImageId& sha)
{
  if (!has_entry (sha))
    return "";

  return _hash[sha]->path;
}

ImageId ImageDatabase::get_sha (const QString& filename)
{
  QString fn = clean_up_filename (filename);

  if (fn.isEmpty ())
    return fn;

  if (_reverse_hash.count (fn) == 0)
    add (fn);

  return _reverse_hash[fn];
}

void ImageDatabase::flush () const
{
  QFile file (_image_db_file);

  if (!file.open (QIODevice::WriteOnly | QIODevice::Text
                  | QIODevice::Append)) {
    qDebug () << "Unable to flush image database to" << _image_db_file;
    return;
  }

  QTextStream s (&file);

  auto b = _hash.begin ();
  auto e = _hash.end ();

  for (; b != e; ++b) {
    if (!(*b)->dirty)
      continue;

    s << b.key () << ':' << (*b)->path << '\n';
    (*b)->dirty = false;
  }
}

ImageDatabase::ImageDatabase ()
{
  set_autoflush ();

  /* Create directory for cached images */
  QDir d (".");

  if (!d.exists (_image_db_dir))
    d.mkdir (_image_db_dir);
}

ImageDatabase::~ImageDatabase ()
{
  for_all (_hash, [] (DBEntry* e) { delete e; });
}

void ImageDatabase::read ()
{
  QFile file (_image_db_file);

  if (!file.open (QIODevice::ReadOnly | QIODevice::Text)) {
    qDebug () << "Unable to read image database from" << _image_db_file;
    return;
  }

  QTextStream s (&file);

  while (!s.atEnd ()) {
    QStringList entry = s.readLine ().split (":");

    add_entry (entry[0], entry[1], false);
  }
}

QString ImageDatabase::clean_up_filename (const QString& filename)
{
  QFileInfo fi (filename);
  return fi.canonicalFilePath ();
}

ImageId ImageDatabase::calculate_sha (const QString& filename)
{
  QFile file (filename);

  if (!file.open (QIODevice::ReadOnly)) {
    qDebug () << "Unable to open file" << filename;
    return ImageId ();
  }

  SHA1 sha;

  while (file.bytesAvailable () > 0) {
    QByteArray bytes = file.read (8096);
    sha.Input (bytes.constData (), bytes.size ());
  }

  return ImageId (sha1_to_string (sha));
}

void ImageDatabase::add_entry (const ImageId& id, const QString& filename,
                               bool dirty)
{
  DBEntry* dbe = new DBEntry (filename);
  dbe->dirty = dirty;
  _hash.insert (id, dbe);
  _reverse_hash.insert (filename, id);
}
