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

#ifndef __TILE__
#define __TILE__

#include <QRect>
#include <QString>
#include <QMap>
#include <QSharedPointer>

#include "uuid.h"

class QPoint;
class QSize;

class Tile
{
public:
  typedef enum {
    PERM_NONE = 0,
    PERM_MOVE = 1,
    PERM_ERASE = 2,
    PERM_ALL = PERM_MOVE | PERM_ERASE
  } Permission;

  typedef enum {
    TILE_IMAGE,
    TILE_CUSTOM
  } TileType;

  Tile (Uuid uuid, const QString& text, TileType type,  int x = 0, int y = 0,
        int w = 1, int h = 1);

  Uuid get_uuid () const { return _uuid; }
  QString get_text () const { return _text; }
  TileType get_type () const { return _type; }
  void add_perm (Uuid uuid, Permission perm);
  void rm_perm (Uuid uuid, Permission perm);
  bool get_perm (Uuid uuid, Permission perm) const;
  void move (int x, int y);
  int get_x () const { return _x; }
  int get_y () const { return _y; }
  int get_width () const { return _width; }
  void set_width (int w) { _width = w; }
  int get_height () const { return _height; }
  void set_height (int h) { _height = h; }

private:
  QString _text;
  Uuid _uuid;
  TileType _type;
  QMap<Uuid, int> _perms;
  int _x;
  int _y;
  int _width;
  int _height;
};

typedef QSharedPointer<Tile> TilePointer;

#endif /* __TILE__ */
