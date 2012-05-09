#include <QDebug>
#include <QPainter>
#include <QRectF>
#include <QGraphicsScene>
#include <QGraphicsObject>
#include <QResizeEvent>
#include <QSize>
#include <QPointF>

#include "game_board.h"
#include "game_tile.h"

GameBoard::GameBoard (QWidget* parent)
  : QGraphicsView (parent),
    _map (0)
{
  _scene = new QGraphicsScene (this);

  setScene (_scene);
  setAlignment (Qt::AlignLeft | Qt::AlignTop);
  setMinimumSize (300, 200);
  setSizePolicy (QSizePolicy::Expanding, QSizePolicy::Expanding);
}

GameBoard::~GameBoard ()
{
  if (_map)
    delete _map;
}

void GameBoard::set_map (QImage* image)
{
  if (_map)
    delete _map;

  _map = image;

  if (image) {
    QSize img_size = image->size ();
    setSceneRect (0, 0, img_size.width (), img_size.height ());
  }
  else {
    setBackgroundBrush (QBrush ());
  }

  _scene->invalidate ();
}

void GameBoard::clear_map ()
{
  set_map (0);
  setSceneRect (0, 0, 0, 0);
}

void GameBoard::add_tile (GameTile* tile)
{
  /* Place larger tiles below smaller ones */
  qreal z = 1.0 / (tile->get_width () * tile->get_height ());
  tile->setZValue (z);
  _scene->addItem (tile);
}

void GameBoard::drawBackground (QPainter* painter, const QRectF& rect)
{
  /* Manually draw image so it doesn't get tiled */
  if (_map) {
    painter->drawImage (rect.topLeft (), *_map, rect);

    QRectF scene_rect = sceneRect ();
    QRectF draw_rect = rect;

    if (draw_rect.width () > scene_rect.width ())
      draw_rect.setWidth (scene_rect.width ());

    if (draw_rect.height () > scene_rect.height ())
      draw_rect.setHeight (scene_rect.height ());

    /* A line with the starting and ending points the same draws nothing so make
     * the minimum dimension 2. */
    if (draw_rect.width () <= 1)
      draw_rect.setWidth (2);

    if (draw_rect.height () <= 1)
      draw_rect.setHeight (2);

    for (int x = ((int)draw_rect.x () + TILE_WIDTH)
                 / (TILE_WIDTH + 1) * (TILE_WIDTH + 1);
         x < draw_rect.x () + draw_rect.width (); x += TILE_WIDTH + 1) {
      painter->drawLine (x, draw_rect.y (),
                         x, draw_rect.y () + draw_rect.height () - 1);
    }

    for (int y = (((int)(draw_rect.y ()) + TILE_HEIGHT)
                 / (TILE_HEIGHT + 1)) * (TILE_HEIGHT + 1);
         y < draw_rect.y () + draw_rect.height (); y += TILE_HEIGHT + 1) {
      painter->drawLine (draw_rect.x (), y,
                         draw_rect.x () + draw_rect.width () - 1, y);
    }
  }
}
