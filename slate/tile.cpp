#include "tile.h"

Tile::Tile (Uuid uuid, const QString& text, TileType type,  int x, int y,
            int w, int h, QObject* parent)
  : QObject (parent),
    QRect (x, y, w, h),
    _text (text),
    _uuid (uuid),
    _type (type)
{
}

Tile::Tile (Uuid uuid, const QString& text, TileType type, const QRect& pos,
            QObject* parent)
  : QObject (parent),
    QRect (pos),
    _text (text),
    _uuid (uuid),
    _type (type)
{
}

Tile::Tile (Uuid uuid, const QString& text, TileType type,
            const QPoint& top_left, const QPoint& bottom_right,
            QObject* parent)
  : QObject (parent),
    QRect (top_left, bottom_right),
    _text (text),
    _uuid (uuid),
    _type (type)
{
}

Tile::Tile (Uuid uuid, const QString& text, TileType type,
            const QPoint& top_left, const QSize& size, QObject* parent)
  : QObject (parent),
    QRect (top_left, size),
    _text (text),
    _uuid (uuid),
    _type (type)
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
