#include "tile.h"

Tile::Tile (Uuid uuid, const QString& text, TileType type,  int x, int y,
            int w, int h, QObject* parent)
  : QObject (parent),
    _text (text),
    _uuid (uuid),
    _type (type),
    _x (x),
    _y (y),
    _width (w),
    _height (h)
{
}

void Tile::add_perm (Uuid uuid, Permission perm)
{
  QMap<Uuid, int>::iterator pos = _perms.find (uuid);

  if (pos == _perms.end ()) {
    _perms.insert (uuid, perm);
    return;
  }

  *pos |= perm;
}

void Tile::rm_perm (Uuid uuid, Permission perm)
{
  QMap<Uuid, int>::iterator pos = _perms.find (uuid);

  if (pos != _perms.end ())
    *pos &= ~perm;
}

bool Tile::get_perm (Uuid uuid, Permission perm) const
{
  QMap<Uuid, int>::const_iterator pos = _perms.find (uuid);

  if (pos == _perms.end ())
    return false;

  return *pos & perm;
}
