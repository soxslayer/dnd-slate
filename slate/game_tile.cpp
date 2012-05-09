#include "game_tile.h"

GameTile::GameTile (Uuid uuid,  QGraphicsItem* parent)
  : QGraphicsPixmapItem (parent),
    _uuid (uuid)
{
  set_x (0);
  set_y (0);
}
