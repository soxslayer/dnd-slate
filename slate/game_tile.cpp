#include <QColor>

#include "game_tile.h"

GameTile::GameTile (Uuid uuid,  QGraphicsItem* parent)
  : QGraphicsPixmapItem (parent),
    _uuid (uuid),
    _selected (false)
{
  set_x (0);
  set_y (0);
}

void GameTile::set_selected (bool selected)
{
  _selected = selected;
  update ();
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
