#ifndef __GAME_TILE__
#define __GAME_TILE__

#include <QGraphicsPixmapItem>

#include "uuid.h"
#include "game_board.h"

class GameTile : public QGraphicsPixmapItem
{
public:
  GameTile (Uuid uuid, QGraphicsItem* parent = 0);

  Uuid get_uuid () const { return _uuid; }
  void set_x (int x) { setX (x * GameBoard::TILE_WIDTH + x + 1); }
  void set_y (int y) { setY (y * GameBoard::TILE_HEIGHT + y + 1); }

  virtual int get_width () const = 0;
  virtual int get_height () const = 0;

private:
  Uuid _uuid;
};

#endif /* __GAME_TILE__ */
