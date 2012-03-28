#include <QDebug>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QString>
#include <QAbstractItemView>

#include "player_list.h"

PlayerList::PlayerList (QWidget* parent)
  : QListView (parent)
{
  _model = new QStandardItemModel (this);

  setModel (_model);
  setEditTriggers (QAbstractItemView::NoEditTriggers);
}

void PlayerList::add_player (Uuid uuid, const QString& name)
{
  QStandardItem* item = new QStandardItem (name);
  _model->appendRow (item);
  _model->sort (0);

  if (_client_id_map.isEmpty ())
    _my_uuid = uuid;

  _client_id_map.insert (uuid, item);
}

void PlayerList::remove_player (Uuid uuid)
{
  QMap<Uuid, QStandardItem*>::iterator i_player = _client_id_map.find (uuid);

  if (i_player == _client_id_map.end ())
    return;

  _model->removeRow ((*i_player)->row ());
  _client_id_map.erase (i_player);
}

void PlayerList::clear ()
{
  QMap<Uuid, QStandardItem*>::iterator beg = _client_id_map.begin ();
  QMap<Uuid, QStandardItem*>::iterator end = _client_id_map.end ();

  for (; beg != end; ++beg) {
    _model->removeRow ((*beg)->row ());
    _client_id_map.erase (beg);
  }
}

QString PlayerList::get_player_name (Uuid uuid) const
{
  QString ret = "<unknown>";
  QMap<Uuid, QStandardItem*>::const_iterator i_player
    = _client_id_map.find (uuid);

  if (i_player != _client_id_map.end ())
    ret = (*i_player)->text ();

  return ret;
}

Uuid PlayerList::get_player_uuid (const QString& name) const
{
  QMap<Uuid, QStandardItem*>::const_iterator beg = _client_id_map.begin ();
  QMap<Uuid, QStandardItem*>::const_iterator end = _client_id_map.end ();

  for (; beg != end; ++beg) {
    if ((*beg)->text () == name)
      return beg.key ();
  }

  return (Uuid)-1;
}
