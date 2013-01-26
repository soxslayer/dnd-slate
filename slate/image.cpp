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

#include <QImage>
#include <QColor>
#include <QByteArray>
#include <QString>

#include "image.h"
#include "dnd_client.h"
#include "image_transfer.h"
#include "game_board.h"
#include "image_database.h"

Image::Image (int width, int height, DnDClient* client)
{
  _image = new QImage (width * (GameBoard::TILE_WIDTH + 1) + 1,
                       height * (GameBoard::TILE_HEIGHT + 1) + 1,
                       QImage::Format_ARGB32);
  _image->fill (QColor (255, 255, 255));

  _transfer = new ImageTransfer (client);
  connect (_transfer, SIGNAL (complete (const QByteArray&)),
    SLOT (transfer_complete (const QByteArray&)));
}

Image::Image (const QString& filename)
{
  _image = new QImage ();
  _image->load (filename);

  _transfer = nullptr;
}

Image::~Image ()
{
  delete _image;

  if (_transfer)
    delete _transfer;
}

int Image::get_width () const
{
  RLOCK ();

  return (_image->width () + (GameBoard::TILE_WIDTH))
         / (GameBoard::TILE_WIDTH + 1);
}

int Image::get_height () const
{
  RLOCK ();

  return (_image->height () + (GameBoard::TILE_HEIGHT))
         / (GameBoard::TILE_HEIGHT + 1);
}

QSize Image::get_size () const
{
  RLOCK ();

  return QSize (get_width (), get_height ());
}

void Image::transfer_complete (const QByteArray& data)
{
  WLOCK ();

  delete _image;
  _image = new QImage (QImage::fromData (data));

  ImageDatabase::get_instance ().add (data);

  changed ();

  _transfer->deleteLater ();
  _transfer = nullptr;
}
