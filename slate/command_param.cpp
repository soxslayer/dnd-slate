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

#include <stdexcept>

#include "command_param.h"

using namespace std;

CommandParam::CommandParam (const string& str)
{
  _param._str = new string (str);
  _type = TypeString;
}

CommandParam::CommandParam (int i)
{
  _param._i = i;
  _type = TypeInt;
}

CommandParam::CommandParam (const CommandParam& p)
{
  _type = TypeInt;
  clone (p);
}

CommandParam::~CommandParam ()
{
  if (_type == TypeString)
    delete _param._str;
}

const CommandParam& CommandParam::operator= (const CommandParam& p)
{
  clone (p);

  return *this;
}

const string& CommandParam::get_str () const
{
  if (_type != TypeString)
    throw runtime_error ("Invalid param type, wanted string is int");

  return *_param._str;
}

int CommandParam::get_int () const
{
  if (_type != TypeInt)
    throw runtime_error ("Invalid param type, wanted int is string");

  return _param._i;
}

void CommandParam::clone (const CommandParam& p)
{
  if (&p != this) {
    if (_type == TypeString)
      delete _param._str;

    _type = p._type;

    switch (_type) {
      case TypeString:
        _param._str = new string (*p._param._str);
        break;

      case TypeInt:
        _param._i = p._param._i;
        break;
    }
  }
}

bool CommandParam::verify_type (char t)
{
  ParamType type;

  switch (t) {
    case 'S':
    case 's':
      type = TypeString;
      break;

    case 'I':
    case 'i':
      type = TypeInt;
      break;

    default:
      return false;
  }

  return type == _type;
}
