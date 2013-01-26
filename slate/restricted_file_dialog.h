#ifndef __RESTRICTED_FILE_DIALOG__
#define __RESTRICTED_FILE_DIALOG__

#include <QFileDialog>
#include <QString>

class RestrictedFileDialog : public QFileDialog
{
  Q_OBJECT;

public:
  RestrictedFileDialog (const QString& root, QWidget* parent = nullptr);

  QString get_selected_file ();
  QString get_selected_file_restricted ();

private:
  QString _root;
  QString _last_dir;

private slots:
  void directory_entered (const QString& dir);
  void verify_input ();
};

#endif /* __RESTRICTED_FILE_DIALOG__ */
