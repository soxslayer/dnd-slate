#include <QApplication>
#include <QFileInfo>
#include <QDebug>

#include "slate_window.h"

int main (int argc, char** argv)
{
  QFileInfo bin_path (argv[0]);
  QApplication app (argc, argv);
  SlateWindow window (bin_path.filePath ());

  window.show ();

  return app.exec ();
}
