#ifndef __IMAGE_TILE__
#define __IMAGE_TILE__

#include "game_tile.h"
#include "tile.h"

class QPixmap;

class ImageTile : public GameTile
{
public:
  ImageTile (const TilePointer& tile, QGraphicsItem* parent = nullptr);

  virtual int get_width () const { return _width; }
  virtual int get_height () const { return _height; }

private:
  QPixmap* _pixmap;
  int _width;
  int _height;
};

#endif /* __IMAGE_TILE__ */
