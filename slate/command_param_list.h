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

#ifndef __COMMAND_PARAM_LIST__
#define __COMMAND_PARAM_LIST__

#include <vector>
#include <string>

class CommandParam;

class CommandParamList
{
public:
  typedef std::vector<CommandParam*>::const_iterator const_iterator;

  static const CommandParamList EMPTY_LIST;

  ~CommandParamList ();

  int size () const { return _params.size (); }
  bool verify_signature (const std::string& sig) const;
  const CommandParam* get_param (int index = 0) const;
  void add_param (const CommandParam& param);

  const_iterator begin () const { return _params.begin (); }
  const_iterator end () const { return _params.end (); }

private:
  typedef std::vector<CommandParam*>::iterator iterator;

  std::vector<CommandParam*> _params;

  iterator begin () { return _params.begin (); }
  iterator end () { return _params.end (); }
};

#endif /* __COMMAND_PARAM_LIST__ */
