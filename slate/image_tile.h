#ifndef __IMAGE_TILE__
#define __IMAGE_TILE__

#include "game_tile.h"

class QPixmap;

class ImageTile : public GameTile
{
public:
  ImageTile (Uuid uuid, const QString& image, QGraphicsItem* parent = 0);

  virtual int get_width () const { return _width; }
  virtual int get_height () const { return _height; }

private:
  QPixmap* _pixmap;
  int _width;
  int _height;
};

#endif /* __IMAGE_TILE__ */
