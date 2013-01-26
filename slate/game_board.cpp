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
#include "custom_tile.h"
#include "image_tile.h"

GameBoard::GameBoard (QWidget* parent)
  : QGraphicsView (parent),
    _map (nullptr),
    _map_size (0, 0),
    _selected_item (nullptr)
{
  _scene = new QGraphicsScene (this);

  setScene (_scene);
  setAlignment (Qt::AlignLeft | Qt::AlignTop);
  setMinimumSize (300, 200);
  setSizePolicy (QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void GameBoard::set_map (const ImagePointer& image)
{
  _map = image;

  if (image) {
    _map_size = image->get_size ();
    setSceneRect (0, 0, _map_size.width () * TILE_WIDTH,
                  _map_size.height () * TILE_HEIGHT);

    connect (_map.data (), SIGNAL (changed ()), SLOT (map_changed ()));
  }
  else {
    _map_size.setWidth (0);
    _map_size.setHeight (0);
    setBackgroundBrush (QBrush ());
  }

  _scene->invalidate ();
}

void GameBoard::clear_map ()
{
  set_map (ImagePointer (nullptr));
  setSceneRect (0, 0, 0, 0);
  delete _scene;
  _scene = new QGraphicsScene (this);
  setScene (_scene);
  _tile_map.clear ();
}

TilePointer GameBoard::get_selected () const
{
  if (!_selected_item)
    return TilePointer ();

  return _selected_item->get_tile ();
}

void GameBoard::add_tile (const TilePointer& tile)
{
  GameTile* g_tile = nullptr;

  switch (tile->get_type ()) {
    case Tile::TILE_CUSTOM:
      g_tile = new CustomTile (tile);
      break;

    case Tile::TILE_IMAGE:
      g_tile = new ImageTile (tile);
      break;
  }

  qreal z = 1.0 / (tile->get_width () * tile->get_height ());
  g_tile->setZValue (z);

  _scene->addItem (g_tile);
  _tile_map.insert (tile->get_uuid (), g_tile);
}

void GameBoard::update_tile (const TilePointer& tile)
{
  auto tile_iter = _tile_map.find (tile->get_uuid ());

  if (tile_iter == _tile_map.end ())
    return;

  (*tile_iter)->update ();
}

void GameBoard::delete_tile (const TilePointer& tile)
{
  auto tile_iter = _tile_map.find (tile->get_uuid ());

  if (tile_iter == _tile_map.end ())
    return;

  if (*tile_iter == _selected_item)
    _selected_item = nullptr;

  _tile_map.remove (tile->get_uuid ());
  _scene->removeItem (*tile_iter);

  delete *tile_iter;
}

void GameBoard::drawBackground (QPainter* painter, const QRectF& rect)
{
  /* Manually draw image so it doesn't get tiled */
  if (_map) {
    painter->drawImage (rect.topLeft (), *_map->get_image (), rect);

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

    if (n_x > _map_size.width () - _selected_item->get_width ())
      n_x = _map_size.width () - _selected_item->get_width ();
    if (n_y > _map_size.height () - _selected_item->get_height ())
      n_y = _map_size.height () - _selected_item->get_height ();

    tile_moved (_selected_item->get_tile (), n_x, n_y);

    _selected_item->set_selected (false);
    _selected_item = 0;
  }
}

void GameBoard::map_changed ()
{
  _scene->invalidate ();
}
