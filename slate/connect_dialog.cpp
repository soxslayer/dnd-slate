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

#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>

#include "connect_dialog.h"

ConnectDialog::ConnectDialog (QWidget* parent)
  : QDialog (parent)
{
  QGridLayout* layout = new QGridLayout (this);

  QLabel* label = new QLabel ("Host", this);
  layout->addWidget (label, 0, 0);

  _host_edit = new QLineEdit (this);
  layout->addWidget (_host_edit, 0, 1);

  label = new QLabel ("Port", this);
  layout->addWidget (label, 1, 0);

  _port_edit = new QLineEdit ("10000", this);
  layout->addWidget (_port_edit, 1, 1);

  label = new QLabel ("Name", this);
  layout->addWidget (label, 2, 0);

  _name_edit = new QLineEdit (this);
  layout->addWidget (_name_edit, 2, 1);

  QPushButton* button = new QPushButton ("&Ok", this);
  button->setDefault (true);
  connect (button, SIGNAL (pressed ()), this, SLOT (verify_input ()));
  layout->addWidget (button, 3, 0);

  button = new QPushButton ("&Cancel", this);
  connect (button, SIGNAL (pressed ()), this, SLOT (reject ()));
  layout->addWidget (button, 3, 1);
}

void ConnectDialog::verify_input ()
{
  if (_port_edit->text ().isEmpty ()) {
    QMessageBox::critical (this, "Error", "Port required");
    return;
  }

  if (_name_edit->text ().isEmpty ()) {
    QMessageBox::critical (this, "Error", "Name required");
    return;
  }

  accept ();
}
