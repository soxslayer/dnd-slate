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

#ifndef __CHAT_WIDGET__
#define __CHAT_WIDGET__

#include <QWidget>
#include <QColor>

#include "player.h"

class QTextEdit;
class QLineEdit;

class ChatWidget : public QWidget
{
  Q_OBJECT

public:
  enum {
    ChatWhisper = 1,
    ChatFromMe = 2
  };

  ChatWidget (QWidget* parent = nullptr);

  void insert_string (const QString& string, QColor& colour,
                      bool new_line = true);
  void insert_status (const QString& status);
  void insert_message (const PlayerPointer& who, const QString& message,
                       int flags);
  void set_entry (const QString& entry);

signals:
  void send_message (const QString& who, const QString& message);

private slots:
  void send_button_pressed ();

private:
  QColor _status_colour;
  QColor _text_colour;
  QColor _whisper_colour;
  QColor _my_message_colour;
  QColor _other_message_colour;
  QTextEdit* _chat_history;
  QLineEdit* _chat_entry;
  bool _first_line;
};

#endif /* __CHAT_WIDGET__ */
