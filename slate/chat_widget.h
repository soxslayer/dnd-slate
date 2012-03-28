#ifndef __CHAT_WIDGET__
#define __CHAT_WIDGET__

#include <QWidget>
#include <QColor>

#include "uuid.h"

class QTextEdit;
class QLineEdit;

class ChatWidget : public QWidget
{
  Q_OBJECT

public:
  enum {
    ChatFromMe = 1,
    ChatWhisper = 2
  };

  ChatWidget (QWidget* parent = 0);

  void insert_string (const QString& string, QColor& colour,
                      bool new_line = true);
  void insert_status (const QString& status);
  void insert_message (const QString& message, const QString& who,
                       int flags);

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
