#ifndef __CONNECT_DIALOG__
#define __CONNECT_DIALOG__

#include <QDialog>
#include <QLineEdit>

class QWidget;

class ConnectDialog : public QDialog
{
  Q_OBJECT

public:
  ConnectDialog (QWidget* parent = 0);

  QString get_host () const { return _host_edit->text (); }
  QString get_port () const { return _port_edit->text (); }
  QString get_name () const { return _name_edit->text (); }

private:
  QLineEdit* _host_edit;
  QLineEdit* _port_edit;
  QLineEdit* _name_edit;
};

#endif /* __CONNECT_DIALOG__ */
