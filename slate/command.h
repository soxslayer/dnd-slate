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

#ifndef __COMMAND__
#define __COMMAND__

#include <QObject>

#include "command_manager.h"

class CommandParamList;

class CommandBase
{
public:
  virtual ~CommandBase () { }

  virtual bool execute (const CommandParamList& params) = 0;
};

template<typename T>
class Command : public CommandBase
{
public:
  typedef bool (T::*CommandCallback)(const CommandParamList&);

  Command (T* obj, CommandCallback callback)
    : _obj (obj), _callback (callback)
  {
  }

  virtual bool execute (const CommandParamList& params)
  {
    return (_obj->*_callback) (params);
  }

private:
  T* _obj;
  CommandCallback _callback;
};

class MarshaledCommand : public QObject, public CommandBase
{
  Q_OBJECT

public:
  MarshaledCommand (CommandBase& base, QObject* parent = 0);

  virtual bool execute (const CommandParamList& params);

signals:
  bool marshal_command (CommandMarshalReceiver::InfoType info);

private:
  CommandBase& _base;
};

#endif /* __COMMAND__ */
