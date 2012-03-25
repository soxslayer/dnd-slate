#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>

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
  connect (button, SIGNAL (pressed ()), this, SLOT (accept ()));
  layout->addWidget (button, 3, 0);

  button = new QPushButton ("&Cancel", this);
  connect (button, SIGNAL (pressed ()), this, SLOT (reject ()));
  layout->addWidget (button, 3, 1);
}
