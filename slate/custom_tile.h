#ifndef __CUSTOM_TILE__
#define __CUSTOM_TILE__

#include <QString>

#include "game_tile.h"
#include "tile.h"

class QPixmap;
class QGraphicsTextItem;

class CustomTile : public GameTile
{
public:
  CustomTile (const TilePointer& tile, QGraphicsItem* parent = nullptr);

  virtual int get_width () const { return get_tile ()->get_width (); }
  virtual int get_height () const { return get_tile ()->get_height (); }

private:
  QPixmap* _bg_pixmap;
  QGraphicsTextItem* _text;
};

#endif /* __CUSTOM_TILE__ */
