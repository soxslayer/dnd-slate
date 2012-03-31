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

#include <QGridLayout>
#include <QTextEdit>
#include <QTextCursor>
#include <QPushButton>
#include <QLineEdit>
#include <QDebug>

#include "chat_widget.h"

ChatWidget::ChatWidget (QWidget* parent)
  : QWidget (parent),
    _status_colour (96, 123, 139), /* LightSkyBlue4 */
    _text_colour (0, 0, 0), /* Black */
    _whisper_colour (105, 89, 205), /* SlateBlue3 */
    _my_message_colour (255, 0, 0), /* Red */
    _other_message_colour (0, 0, 255), /* Blue */
    _first_line (true)
{
  QGridLayout* layout = new QGridLayout (this);
  setLayout (layout);

  _chat_history = new QTextEdit (this);
  _chat_history->setReadOnly (true);
  layout->addWidget (_chat_history, 0, 0, 1, 2);

  _chat_entry = new QLineEdit (this);
  connect (_chat_entry, SIGNAL (returnPressed ()),
           this, SLOT (send_button_pressed ()));
  layout->addWidget (_chat_entry, 1, 0);

  QPushButton* send_button = new QPushButton ("Send", this);
  send_button->setDefault (true);
  connect (send_button, SIGNAL (pressed ()),
           this, SLOT (send_button_pressed ()));
  layout->addWidget (send_button, 1, 1);
}

void ChatWidget::insert_string (const QString& string, QColor& colour,
                                bool new_line)
{
  _chat_history->moveCursor (QTextCursor::End);

  if (new_line && !_first_line)
    _chat_history->insertPlainText ("\n");

  _first_line = false;

  _chat_history->setTextColor (colour);
  _chat_history->insertPlainText (string);
}

void ChatWidget::insert_status (const QString& status)
{
  insert_string ("* ", _status_colour);
  insert_string (status, _status_colour, false);
  insert_string (" *", _status_colour, false);
}

void ChatWidget::insert_message (const QString& message, const QString& who,
                                 int flags)
{
  if (flags & ChatFromMe && flags & ChatWhisper) {
    insert_string ("*** TO ", _whisper_colour);
    insert_string (who, _whisper_colour, false);
    insert_string (": ", _whisper_colour, false);
    insert_string (message, _whisper_colour, false);
  }
  else if (flags & ChatFromMe) {
    insert_string (who, _my_message_colour);
    insert_string (": ", _my_message_colour, false);
    insert_string (message, _text_colour, false);
  }
  else if (flags & ChatWhisper) {
    insert_string ("*** ", _whisper_colour);
    insert_string (who, _whisper_colour, false);
    insert_string (": ", _whisper_colour, false);
    insert_string (message, _whisper_colour, false);
  }
  else {
    insert_string (who, _other_message_colour);
    insert_string (": ", _other_message_colour, false);
    insert_string (message, _text_colour, false);
  }
}

void ChatWidget::set_entry (const QString& entry)
{
  _chat_entry->clear ();
  _chat_entry->insert (entry);
  _chat_entry->setFocus ();
}

void ChatWidget::send_button_pressed ()
{
  QString msg = _chat_entry->text ();
  QString name;

  if (msg.isEmpty ())
    return;

  if (msg.startsWith ("/w")) {
    /* Remove the leading "/w" */
    int i = msg.indexOf (' ');
    msg.remove (0, i + 1);

    i = msg.indexOf (' ');
    name = msg.left (i);
    msg.remove (0, i + 1);
  }

  _chat_entry->clear ();
  send_message (name, msg);
}
