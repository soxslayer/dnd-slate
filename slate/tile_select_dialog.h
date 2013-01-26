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

#ifndef __TILE_SELECT_DIALOG__
#define __TILE_SELECT_DIALOG__

#include <QDialog>

class QWidget;
class QFrame;
class QLineEdit;

class TileSelectDialog : public QDialog
{
  Q_OBJECT

public:
  typedef enum {
    SelectLoad,
    SelectCustom
  } SelectionType;

  TileSelectDialog (QWidget* parent = nullptr);

  SelectionType get_selection_type () const;
  QString get_filename () const;
  int get_width () const;
  int get_height () const;
  QString get_text () const;

private slots:
  void load_tile_toggled (bool checked);
  void custom_tile_toggled (bool checked);
  void choose_button_pressed ();
  void verify_input ();

private:
  QFrame* _load_tile_frame;
  QFrame* _custom_tile_frame;
  QLineEdit* _load_tile_line_edit;
  QLineEdit* _width_line_edit;
  QLineEdit* _height_line_edit;
  QLineEdit* _text_line_edit;
};

#endif /* __TILE_SELECT_DIALOG__ */
