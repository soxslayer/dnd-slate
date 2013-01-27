#include <QColor>

#include "game_tile.h"
#include "game_board.h"

GameTile::GameTile (const TilePointer& tile,  QGraphicsItem* parent)
  : QGraphicsPixmapItem (parent),
    _tile (tile),
    _selected (false)
{
  update ();
}

void GameTile::update ()
{
  setX (_tile->get_x () * (GameBoard::TILE_WIDTH + 1) + 1);
  setY (_tile->get_y () * (GameBoard::TILE_HEIGHT + 1) + 1);
  QGraphicsItem::update ();
}

void GameTile::set_selected (bool selected)
{
  _selected = selected;
  QGraphicsItem::update ();
}

void GameTile::paint (QPainter* painter,
                      const QStyleOptionGraphicsItem* option,
                      QWidget* widget)
{
  QGraphicsPixmapItem::paint (painter, option, widget);

  if (_selected) {
    painter->setCompositionMode (QPainter::CompositionMode_SourceAtop);
    painter->setOpacity (.25);
    painter->fillRect (boundingRect (), QColor (0, 0, 255));
  }
}
