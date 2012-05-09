#include <QDir>
#include <QDebug>

#include "restricted_file_dialog.h"

RestrictedFileDialog::RestrictedFileDialog (const QString& root,
                                            QWidget* parent)
  : QFileDialog (parent),
    _root (QDir::cleanPath (root)),
    _last_dir (_root)
{
  connect (this, SIGNAL (directoryEntered (const QString&)),
           this, SLOT (directory_entered (const QString&)));
  connect (this, SIGNAL (accepted ()), this, SLOT (verify_input ()));

  setDirectory (_root);
}

QString RestrictedFileDialog::get_selected_file ()
{
  QStringList selected_files = selectedFiles ();
  QStringList::iterator file = selected_files.begin ();
  return *file;
}

QString RestrictedFileDialog::get_selected_file_restricted ()
{
  QString f = get_selected_file ();
  f.remove (_root);

  while (f.startsWith ("/"))
    f.remove (0, 1);

  return f;
}

void RestrictedFileDialog::directory_entered (const QString& dir)
{
  QString clean_dir = QDir::cleanPath (dir);

  if (clean_dir == _root || QDir::match (_root + "/*", clean_dir))
    _last_dir = clean_dir;
  else
    setDirectory (_last_dir);
}

void RestrictedFileDialog::verify_input ()
{
  QString clean_path = QDir::cleanPath (get_selected_file ());

  if (!QDir::match (_root + "/*", clean_path))
    reject ();
}
