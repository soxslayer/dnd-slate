#ifndef __TILE__
#define __TILE__

#include <QObject>
#include <QRect>
#include <QString>
#include <QMap>

#include "uuid.h"

class QPoint;
class QSize;

class Tile : public QObject
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

  Uuid get_uuid () const { return _uuid; }
  QString get_text () const { return _text; }
  TileType get_type () const { return _type; }
  void add_perm (Uuid uuid, Permission perm);
  void rm_perm (Uuid uuid, Permission perm);
  bool get_perm (Uuid uuid, Permission perm) const;
  int get_x () const { return _x; }
  void set_x (int x) { _x = x; }
  int get_y () const { return _y; }
  void set_y (int y) { _y = y; }
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

#endif /* __TILE__ */
