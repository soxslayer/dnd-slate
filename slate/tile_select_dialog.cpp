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
#include <QGridLayout>
#include <QRadioButton>
#include <QLineEdit>
#include <QPushButton>
#include <QFrame>
#include <QBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QDir>

#include "tile_select_dialog.h"
#include "restricted_file_dialog.h"

TileSelectDialog::TileSelectDialog (QWidget* parent)
  : QDialog (parent)
{
  QBoxLayout* layout = new QBoxLayout (QBoxLayout::TopToBottom, this);

  QRadioButton* radio_button = new QRadioButton ("Load Tile", this);
  radio_button->setChecked (true);
  layout->addWidget (radio_button);
  connect (radio_button, SIGNAL (toggled (bool)),
           this, SLOT (load_tile_toggled (bool)));

  _load_tile_frame = new QFrame (this);
  layout->addWidget (_load_tile_frame);

  QBoxLayout* box_layout = new QBoxLayout (QBoxLayout::LeftToRight,
                                           _load_tile_frame);

  _load_tile_line_edit = new QLineEdit (_load_tile_frame);
  box_layout->addWidget (_load_tile_line_edit);

  QPushButton* button = new QPushButton ("Choose", _load_tile_frame);
  box_layout->addWidget (button);
  connect (button, SIGNAL (pressed ()),
           this, SLOT (choose_button_pressed ()));

  radio_button = new QRadioButton ("Custom Tile", this);
  layout->addWidget (radio_button);

  _custom_tile_frame = new QFrame (this);
  _custom_tile_frame->setEnabled (false);
  layout->addWidget (_custom_tile_frame);
  connect (radio_button, SIGNAL (toggled (bool)),
           this, SLOT (custom_tile_toggled (bool)));

  QGridLayout* grid_layout = new QGridLayout (_custom_tile_frame);

  grid_layout->addWidget (new QLabel ("Size", _custom_tile_frame), 0, 0);

  _width_line_edit = new QLineEdit ("1", _custom_tile_frame);
  grid_layout->addWidget (_width_line_edit, 1, 0);

  grid_layout->addWidget (new QLabel ("x", _custom_tile_frame), 1, 1);

  _height_line_edit = new QLineEdit ("1", _custom_tile_frame);
  grid_layout->addWidget (_height_line_edit, 1, 2);

  grid_layout->addWidget (new QLabel ("Text", _custom_tile_frame), 2, 0);

  _text_line_edit = new QLineEdit (_custom_tile_frame);
  grid_layout->addWidget (_text_line_edit, 3, 0, 1, 3);

  box_layout = new QBoxLayout (QBoxLayout::RightToLeft);
  layout->addLayout (box_layout);

  button = new QPushButton ("&Ok", this);
  button->setDefault (true);
  connect (button, SIGNAL (pressed ()), this, SLOT (verify_input ()));
  box_layout->addWidget (button);

  button = new QPushButton ("&Cancel", this);
  connect (button, SIGNAL (pressed ()), this, SLOT (reject ()));
  box_layout->addWidget (button);
}

TileSelectDialog::SelectionType
TileSelectDialog::get_selection_type () const
{
  return _load_tile_frame->isEnabled () ? SelectLoad : SelectCustom;
}

QString TileSelectDialog::get_filename () const
{
  return _load_tile_line_edit->text ();
}

int TileSelectDialog::get_width () const
{
  return _width_line_edit->text ().toInt ();
}

int TileSelectDialog::get_height () const
{
  return _height_line_edit->text ().toInt ();;
}

QString TileSelectDialog::get_text () const
{
  return _text_line_edit->text ();
}

void TileSelectDialog::load_tile_toggled (bool checked)
{
  _load_tile_frame->setEnabled (checked);
}

void TileSelectDialog::custom_tile_toggled (bool checked)
{
  _custom_tile_frame->setEnabled (checked);
}

void TileSelectDialog::choose_button_pressed ()
{
  QDir image_root ("image:.");
  RestrictedFileDialog rfd (image_root.path (), this);
  if (rfd.exec ())
    _load_tile_line_edit->setText (rfd.get_selected_file_restricted ());
}

void TileSelectDialog::verify_input ()
{
  if (_load_tile_frame->isEnabled ()) {
    if (_load_tile_line_edit->text ().isEmpty ()) {
      QMessageBox::critical (this, "Error", "Tile required");
      return;
    }
  }
  else {
    if (_width_line_edit->text ().isEmpty ()) {
      QMessageBox::critical (this, "Error", "Width required");
      return;
    }

    if (_height_line_edit->text ().isEmpty ()) {
      QMessageBox::critical (this, "Error", "Height required");
      return;
    }

    if (_text_line_edit->text ().isEmpty ()) {
      QMessageBox::critical (this, "Error", "Text required");
      return;
    }
  }

  accept ();
}
