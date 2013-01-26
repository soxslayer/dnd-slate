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

#include <QByteArray>

#include "image_transfer.h"
#include "dnd_messages.h"
#include "dnd_client.h"

ImageTransfer::ImageTransfer (DnDClient* client, QObject* parent)
  : QObject (parent)
{
  connect (client, SIGNAL (image_begin (DnDClient*, quint64)),
    SLOT (image_begin (DnDClient*, quint64)));
  connect (client,
    SIGNAL (image_data (DnDClient*, quint32, const QByteArray&)),
    SLOT (image_data (DnDClient*, quint32, const QByteArray&)));
}

void ImageTransfer::image_begin (DnDClient*, quint64 total_size)
{
  _data.reserve (total_size);
  _total_size = total_size;
}

void ImageTransfer::image_data (DnDClient* client, quint32 sequence,
                                const QByteArray& data)
{
  quint32 offset = sequence * DND_IMAGE_MAX_CHUNK_SIZE;

  _data.replace (offset, data.size (), data);

  if (offset + data.size () >= _total_size)
    complete (_data);
}
