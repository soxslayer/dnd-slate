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
    _map (0),
    _selected_item (0)
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
    _width = (img_size.width () + TILE_WIDTH - 1) / (TILE_WIDTH + 1);
    _height = (img_size.height () + TILE_HEIGHT - 1) / (TILE_HEIGHT + 1);
  }
  else {
    _width = 0;
    _height = 0;
    setBackgroundBrush (QBrush ());
  }

  _scene->invalidate ();
}

void GameBoard::clear_map ()
{
  set_map (0);
  setSceneRect (0, 0, 0, 0);
  delete _scene;
  _scene = new QGraphicsScene (this);
  setScene (_scene);
  _tile_map.clear ();
}

void GameBoard::add_tile (GameTile* tile)
{
  /* Place larger tiles below smaller ones */
  qreal z = 1.0 / (tile->get_width () * tile->get_height ());
  tile->setZValue (z);
  _scene->addItem (tile);
  _tile_map.insert (tile->get_uuid (), tile);
}

GameTile* GameBoard::delete_tile (Uuid uuid)
{
  QMap<Uuid, GameTile*>::iterator tile_iter = _tile_map.find (uuid);

  if (tile_iter == _tile_map.end ())
    return NULL;

  GameTile* tile = *tile_iter;

  if (tile == _selected_item)
    _selected_item = 0;

  _tile_map.remove (uuid);
  _scene->removeItem (tile);

  return tile;
}

Uuid GameBoard::get_selected_uuid () const
{
  if (!_selected_item)
    return UuidManager::UUID_INVALID;

  return _selected_item->get_uuid ();
}

void GameBoard::move_tile (Uuid uuid, int x, int y)
{
  QMap<Uuid, GameTile*>::iterator tile_iter = _tile_map.find (uuid);

  if (tile_iter == _tile_map.end ())
    return;

  GameTile* tile = *tile_iter;

  tile->set_x (x);
  tile->set_y (y);
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

void GameBoard::mousePressEvent (QMouseEvent* event)
{
  if (!_selected_item) {
    QGraphicsItem* item = itemAt (event->pos ());
    if (item) {
      while (item->parentItem ())
        item = item->parentItem ();

      _selected_item = reinterpret_cast<GameTile*> (item);
      _selected_item->set_selected ();
    }
  }
  else {
    QPointF scene_pos = mapToScene (event->pos ());
    int n_x = scene_pos.x () / (TILE_WIDTH + 1);
    int n_y = scene_pos.y () / (TILE_HEIGHT + 1);

    if (n_x > _width - _selected_item->get_width ())
      n_x = _width - _selected_item->get_width ();
    if (n_y > _height - _selected_item->get_height ())
      n_y = _height - _selected_item->get_height ();

    tile_moved (_selected_item->get_uuid (), n_x, n_y);

    _selected_item->set_selected (false);
    _selected_item = 0;
  }
}
