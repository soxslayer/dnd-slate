/* Copyright (c) 2013, Dustin Mitchell dmmitche <at> gmail <dot> com
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
#include <QModelIndex>

#include "player_list.h"

Q_DECLARE_METATYPE (PlayerPointer);

PlayerList::PlayerList (QWidget* parent)
  : QListView (parent)
{
  _model = new QStandardItemModel (this);

  setModel (_model);
  setEditTriggers (QAbstractItemView::NoEditTriggers);

  connect (this, SIGNAL (doubleClicked (const QModelIndex&)),
           SLOT (item_double_clicked (const QModelIndex&)));

  setMaximumWidth (150);
  setSizePolicy (QSizePolicy::Minimum, QSizePolicy::Minimum);
}

void PlayerList::add_player (const PlayerPointer& player)
{
  QStandardItem* item = new QStandardItem (player->get_name ());
  item->setData (QVariant::fromValue (player), Qt::UserRole);

  _model->appendRow (item);
  _model->sort (0);
}

void PlayerList::remove_player (const PlayerPointer& player)
{
  for (int i = 0; i < _model->rowCount (); ++i) {
    QModelIndex idx = _model->index (i, 0);
    PlayerPointer p = idx.data (Qt::UserRole).value<PlayerPointer> ();

    if (p->get_uuid () == player->get_uuid ()) {
      _model->removeRow (i);
      break;
    }
  }
}

void PlayerList::clear ()
{
  _model->clear ();
}

void PlayerList::item_double_clicked (const QModelIndex& index)
{
  player_activated (index.data (Qt::UserRole).value<PlayerPointer> ());
}
