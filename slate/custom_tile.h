#ifndef __CUSTOM_TILE__
#define __CUSTOM_TILE__

#include <QString>

#include "game_tile.h"

class QPixmap;
class QGraphicsTextItem;

class CustomTile : public GameTile
{
public:
  CustomTile (Uuid uuid, int width, int height, const QString& text,
              QGraphicsItem* parent = 0);

  virtual int get_width () const { return _width; }
  virtual int get_height () const { return _height; }

private:
  QPixmap* _bg_pixmap;
  int _width;
  int _height;
  QGraphicsTextItem* _text;
};

#endif /* __CUSTOM_TILE__ */
