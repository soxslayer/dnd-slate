#ifndef __GAME_TILE__
#define __GAME_TILE__

#include <QGraphicsPixmapItem>

#include "tile.h"

class GameTile : public QGraphicsPixmapItem
{
public:
  GameTile (const TilePointer& tile, QGraphicsItem* parent = 0);

  const TilePointer& get_tile () const { return _tile; }
  void update ();
  void set_selected (bool selected = true);

  virtual int get_width () const = 0;
  virtual int get_height () const = 0;

protected:
  virtual void paint (QPainter* painter,
                      const QStyleOptionGraphicsItem* option,
                      QWidget* widget);

private:
  TilePointer _tile;
  bool _selected;
};

#endif /* __GAME_TILE__ */
