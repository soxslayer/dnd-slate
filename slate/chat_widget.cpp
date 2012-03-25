#include <QGridLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QLineEdit>
#include <QDebug>

#include "chat_widget.h"

ChatWidget::ChatWidget (QWidget* parent)
  : QWidget (parent),
    _status_colour (96, 123, 139), /* LightSkyBlue4 */
    _message_colour (0, 0, 0),
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

void ChatWidget::insert_status (const QString& status)
{
  if (!_first_line)
    _chat_history->append ("\n");

  QString s = "* ";
  s += status;
  s += " *";

  _chat_history->setTextColor (_status_colour);
  _chat_history->append (s);
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

  qDebug () << name << msg;
}
