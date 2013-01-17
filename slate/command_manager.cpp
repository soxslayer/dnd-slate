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

#include "command_manager.h"
#include "command.h"

using namespace std;

CommandMarshalReceiver::CommandMarshalReceiver (QObject* parent)
  : QObject (parent)
{
}

bool CommandMarshalReceiver::receive_command (InfoType info)
{
  return info.first->execute (*info.second);
}



void CommandManager::add_command (const string& name, CommandBase* cmd)
{
  CommandManager* mgr = get_instance ();

  if (mgr->_cmds.count (name) > 0)
    return;

  mgr->_cmds.insert (pair<string, CommandBase*> (name, cmd));
}

CommandBase& CommandManager::get_command (const string& name)
{
  CommandManager* mgr = get_instance ();

  return *mgr->_cmds.at (name);
}

CommandManager* CommandManager::_instance = 0;

CommandManager* CommandManager::get_instance ()
{
  if (!_instance)
    _instance = new CommandManager;

  return _instance;
}
