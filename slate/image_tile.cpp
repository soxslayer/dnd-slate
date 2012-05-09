#include <QPixmap>

#include "image_tile.h"
#include "game_board.h"

ImageTile::ImageTile (Uuid uuid, const QString& image, QGraphicsItem* parent)
  : GameTile (uuid, parent)
{
  _pixmap = new QPixmap (image);

  _width = (_pixmap->width () + GameBoard::TILE_WIDTH - 1 )
           / GameBoard::TILE_WIDTH;
  _height = (_pixmap->height () + GameBoard::TILE_HEIGHT - 1)
            / GameBoard::TILE_WIDTH;

  setPixmap (*_pixmap);
}
