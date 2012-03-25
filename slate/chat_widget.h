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
  ChatWidget (QWidget* parent = 0);

  void insert_status (const QString& status);

signals:
  void send_message (const QString& who, const QString& message);

private slots:
  void send_button_pressed ();

private:
  QColor _status_colour;
  QColor _message_colour;
  QTextEdit* _chat_history;
  QLineEdit* _chat_entry;
  bool _first_line;
};

#endif /* __CHAT_WIDGET__ */
