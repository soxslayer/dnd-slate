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

#ifndef __COMMAND_MANAGER__
#define __COMMAND_MANAGER__

#include <string>
#include <map>

#include <QObject>
#include <QPair>

class CommandBase;
class CommandParamList;

class CommandMarshalReceiver : public QObject
{
  Q_OBJECT

public:
  typedef QPair<CommandBase*, const CommandParamList*> InfoType;

  CommandMarshalReceiver (QObject* parent = nullptr);

public slots:
  bool receive_command (CommandMarshalReceiver::InfoType info);
};



class CommandManager
{
public:
  typedef std::map<std::string, CommandBase*>::iterator iterator;
  typedef std::map<std::string, CommandBase*>::const_iterator const_iterator;

  static void init () { get_instance (); }
  static void add_command (const std::string& name, CommandBase* cmd);
  static CommandBase& get_command (const std::string& name);
  template<typename T>
  static T& get_typed_command (const std::string& name)
  {
    CommandManager* mgr = get_instance ();
    return dynamic_cast<T&> (get_command (name));
  }
  static iterator begin () { return get_instance ()->_cmds.begin (); }
  static const_iterator c_begin () { return get_instance ()->_cmds.begin (); }
  static iterator end () { return get_instance ()->_cmds.end (); }
  static const_iterator c_end () { return get_instance ()->_cmds.end (); }
  static CommandMarshalReceiver& get_marshal_receiver ()
  { return get_instance ()->_marshal_receiver; }

private:
  static CommandManager* _instance;

  static CommandManager* get_instance ();

  std::map<std::string, CommandBase*> _cmds;
  CommandMarshalReceiver _marshal_receiver;

  CommandManager () { }
  CommandManager (const CommandManager&) { }
  const CommandManager& operator= (const CommandManager&) { return *this; }
};

#endif /* __COMMAND_MANAGER__ */
