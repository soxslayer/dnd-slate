#ifndef __GAME_BOARD__
#define __GAME_BOARD__

#include <QGraphicsView>

class QWidget;
class QPainter;
class QRectF;
class QGraphicsScene;
class GameTile;

class GameBoard : public QGraphicsView
{
public:
  enum { TILE_WIDTH = 32, TILE_HEIGHT = 32 };

  GameBoard (QWidget* parent = 0);
  ~GameBoard ();

  void set_map (QImage* image);
  void clear_map ();
  void add_tile (GameTile* tile);

protected:
  virtual void drawBackground (QPainter* painter, const QRectF& rect);

private:

  QGraphicsScene* _scene;
  QImage* _map;
};

#endif /* __GAME_BOARD__ */
