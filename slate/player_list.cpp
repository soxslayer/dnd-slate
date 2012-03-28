/* Copyright (c) 2012, Dustin Mitchell dmmitche <at> gmail <dot> com
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the document
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

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
