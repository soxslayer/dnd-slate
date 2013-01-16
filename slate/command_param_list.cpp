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

#include "command_param_list.h"
#include "command_param.h"

using namespace std;

const CommandParamList CommandParamList::EMPTY_LIST;

CommandParamList::~CommandParamList ()
{
  iterator p = begin ();
  iterator p_end = end ();

  for (; p != p_end; ++p)
    delete *p;
}

bool CommandParamList::verify_signature (const string& sig) const
{
  string::const_iterator c = sig.begin ();
  string::const_iterator c_end = sig.end ();
  const_iterator p = begin ();
  const_iterator p_end = end ();
  bool verified = true;

  for (; c != c_end && p != p_end && verified; ++c, ++p)
    verified = (*p)->verify_type (*c);

  if (c != c_end || p != p_end)
    verified = false;

  return verified;
}

const CommandParam* CommandParamList::get_param (int index) const
{
  if (index >= _params.size ())
    return 0;

  return _params[index];
}

void CommandParamList::add_param (const CommandParam& param)
{
  _params.push_back (new CommandParam (param));
}
