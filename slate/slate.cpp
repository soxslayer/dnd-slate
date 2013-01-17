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

#include <QApplication>
#include <QProcessEnvironment>
#include <QMessageBox>
#include <QDir>

#include "slate_window.h"
#include "command_manager.h"
#include "lua_bindings.h"

int main (int argc, char** argv)
{
  QApplication app (argc, argv);
  SlateWindow window;

  CommandManager::init ();

  if (QProcessEnvironment::systemEnvironment ().contains ("DND_SLATE_IMAGES"))
    QDir::addSearchPath ("image",
      QProcessEnvironment::systemEnvironment ().value ("DND_SLATE_IMAGES"));
  else
    QDir::addSearchPath ("image",
      QCoreApplication::applicationDirPath () + "/images");

  QDir images_test ("image:.");
  if (!images_test.exists ()) {
    QMessageBox::critical (0, "Error", "Cannot find image directory. "
                     "Try setting DND_SLATE_IMAGES in your environment.");
    return 1;
  }

  window.show ();

  if (argc > 1) {
    for (int i = 1; i < argc; ++i)
      lua_run_script_async (argv[i]);
  }

  return app.exec ();
}
