#ifndef __GAME_BOARD__
#define __GAME_BOARD__

#include <QGraphicsView>
#include <QMap>
#include <QSize>

#include "uuid.h"
#include "tile.h"
#include "image.h"

class QWidget;
class QPainter;
class QRectF;
class QGraphicsScene;
class QMouseEvent;
class GameTile;

class GameBoard : public QGraphicsView
{
  Q_OBJECT

public:
  enum { TILE_WIDTH = 32, TILE_HEIGHT = 32 };

  GameBoard (QWidget* parent = nullptr);

  void set_map (const ImagePointer& image);
  void clear_map ();
  TilePointer get_selected () const;

signals:
  void tile_moved (const TilePointer& tile, int x, int y);

public slots:
  void add_tile (const TilePointer& tile);
  void update_tile (const TilePointer& tile);
  void delete_tile (const TilePointer& tile);

protected:
  virtual void drawBackground (QPainter* painter, const QRectF& rect);
  virtual void mousePressEvent (QMouseEvent* event);

private slots:
  void map_changed ();

private:
  QGraphicsScene* _scene;
  ImagePointer _map;
  QSize _map_size;
  GameTile* _selected_item;
  QMap<Uuid, GameTile*> _tile_map;
  int _width;
  int _height;
};

#endif /* __GAME_BOARD__ */
