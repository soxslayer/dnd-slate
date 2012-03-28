#ifndef __PLAYER_LIST__
#define __PLAYER_LIST__

#include <QListView>
#include <QMap>

#include "uuid.h"

class QString;
class QWidget;
class QStandardItem;
class QStandardItemModel;

class PlayerList : public QListView
{
public:
  PlayerList (QWidget* parent = 0);

  void add_player (Uuid uuid, const QString& name);
  void remove_player (Uuid uuid);
  void clear ();
  QString get_player_name (Uuid) const;
  Uuid get_player_uuid (const QString& name) const;
  QString get_my_name () const { return get_player_name (_my_uuid); }
  Uuid get_my_uuid () const { return _my_uuid; }

private:
  QStandardItemModel* _model;
  Uuid _my_uuid;
  QMap<Uuid, QStandardItem*> _client_id_map;
};

#endif /* __PLAYER_LIST__ */
