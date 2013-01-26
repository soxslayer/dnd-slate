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

#include "tile.h"

Tile::Tile (Uuid uuid, const QString& text, TileType type,  int x, int y,
            int w, int h)
 :  DnDObject (uuid),
    _text (text),
    _type (type),
    _x (x),
    _y (y),
    _width (w),
    _height (h)
{
}

void Tile::add_perm (Uuid uuid, Permission perm)
{
  WLOCK();

  auto pos = _perms.find (uuid);

  if (pos == _perms.end ()) {
    _perms.insert (uuid, perm);
    return;
  }

  *pos |= perm;
}

void Tile::rm_perm (Uuid uuid, Permission perm)
{
  WLOCK();

  auto pos = _perms.find (uuid);

  if (pos != _perms.end ())
    *pos &= ~perm;
}

bool Tile::get_perm (Uuid uuid, Permission perm) const
{
  RLOCK();

  auto pos = _perms.find (uuid);

  if (pos == _perms.end ())
    return false;

  return *pos & perm;
}

void Tile::move (int x, int y)
{
  WLOCK();

  _x = x;
  _y = y;
}
