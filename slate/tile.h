#ifndef __TILE__
#define __TILE__

#include <QObject>
#include <QRect>
#include <QString>
#include <QMap>

#include "uuid.h"

class QPoint;
class QSize;

class Tile : public QObject, public QRect
{
  Q_OBJECT;

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
        int w = 1, int h = 1, QObject* parent = 0);
  Tile (Uuid uuid, const QString& text, TileType type,  const QRect& pos,
        QObject* parent = 0);
  Tile (Uuid uuid, const QString& text, TileType type, const QPoint& top_left,
        const QPoint& bottom_right, QObject* parent = 0);
  Tile (Uuid uuid, const QString& text, TileType type,
        const QPoint& top_left, const QSize& size, QObject* parent = 0);

  Uuid get_uuid () const { return _uuid; }
  QString get_text () const { return _text; }
  TileType get_type () const { return _type; }
  void add_perm (Uuid uuid, Permission perm);
  void rm_perm (Uuid uuid, Permission perm);
  bool get_perm (Uuid uuid, Permission perm) const;

private:
  QString _text;
  Uuid _uuid;
  TileType _type;
  QMap<Uuid, int> _perms;
};

#endif /* __TILE__ */
