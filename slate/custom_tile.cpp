#include <QDebug>
#include <QPixmap>
#include <QBitmap>
#include <QPainter>
#include <QBrush>
#include <QColor>
#include <QGraphicsTextItem>
#include <QRectF>

#include "custom_tile.h"
#include "game_board.h"

CustomTile::CustomTile (const TilePointer& tile, QGraphicsItem* parent)
  : GameTile (tile, parent)
{
  int t_width = GameBoard::TILE_WIDTH * tile->get_width ()
                + tile->get_width () - 1;
  int t_height = GameBoard::TILE_HEIGHT * tile->get_height ()
                 + tile->get_height () - 1;

  _bg_pixmap = new QPixmap (t_width, t_height);
  _bg_pixmap->fill ();
  _bg_pixmap->setMask (_bg_pixmap->createHeuristicMask ());

  QPainter painter (_bg_pixmap);

  painter.setBrush (QBrush (QColor (255, 255, 255)));
  painter.setPen (QColor (0, 0, 0));
  painter.drawRoundedRect (0, 0, t_width - 1, t_height - 1, 7, 7);

  setPixmap (*_bg_pixmap);

  GraphicsItemFlags f = flags () | QGraphicsItem::ItemClipsChildrenToShape;
  setFlags (f);

  _text = new QGraphicsTextItem (tile->get_text (), this);

  QRectF text_bound = _text->boundingRect ();
  QRectF my_bound = boundingRect ();

  int c_x, c_y;

  c_x = (my_bound.width () - text_bound.width ()) / 2;
  if (c_x < 0)
    c_x = 0;

  c_y = (my_bound.height () - text_bound.height ()) / 2;

  _text->setX (c_x);
  _text->setY (c_y);
}
