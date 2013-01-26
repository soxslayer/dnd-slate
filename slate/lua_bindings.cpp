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

#include <QDebug>
#include <QRunnable>
#include <QThreadPool>
#include <lua.hpp>

#include "lua_bindings.h"
#include "command.h"
#include "command_manager.h"
#include "command_param.h"
#include "command_param_list.h"

using namespace std;

extern "C" int lua_command_callback (lua_State* L);

static void lua_register_command (lua_State* L, const string& name,
                                  const CommandBase* cmd)
{
  lua_pushlightuserdata (L, (void*)cmd);
  lua_pushcclosure (L, lua_command_callback, 1);
  lua_setglobal (L, name.c_str ());
}

static void lua_open_command_lib (lua_State* L)
{
  CommandManager::const_iterator cmd = CommandManager::c_begin ();
  CommandManager::const_iterator cmd_end = CommandManager::c_end ();

  for (; cmd != cmd_end; ++cmd)
    lua_register_command (L, cmd->first, cmd->second);
}

static bool lua_get_async_context (lua_State* L)
{
  lua_rawgeti (L, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
  lua_pushstring (L, "__async");
  lua_gettable (L, -2);
  bool async = lua_tointeger (L, -1);
  lua_pop (L, 1);
  return async;
}

static void lua_set_async_context (lua_State* L, bool async)
{
  lua_rawgeti (L, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
  lua_pushstring (L, "__async");
  lua_pushinteger (L, async);
  lua_settable (L, -3);
  lua_pop (L, 1);
}

static bool lua_start_script (const string& script, bool async = false)
{
  lua_State* L;

  L = luaL_newstate ();
  if (L == NULL)
    return false;

  luaL_openlibs (L);
  lua_open_command_lib (L);
  lua_set_async_context (L, async);

  if (luaL_loadfile (L, script.c_str ()) != LUA_OK) {
    qDebug () << "Error loading LUA script: " << lua_tostring (L, -1);
    lua_close (L);
    return false;
  }

  if (lua_pcall (L, 0, 0, 0) != LUA_OK) {
    qDebug () << "Error running LUA script: " << lua_tostring (L, -1);
    lua_close (L);
    return false;
  }

  lua_close (L);

  return true;
}



extern "C" int lua_command_callback (lua_State* L)
{
  CommandBase* cmd_ptr = static_cast<CommandBase*> (lua_touserdata (L,
                                                    lua_upvalueindex (1)));
  CommandParamList params;
  int n = lua_gettop (L);

  for (int i = 1; i <= n; ++i) {
    if (lua_isnumber (L, i)) {
      CommandParam p ((int)lua_tointeger (L, i));
      params.add_param (p);
    }
    else if (lua_isstring (L, i)) {
      CommandParam p (lua_tostring (L, i));
      params.add_param (p);
    }
    else {
      qDebug () << "Invalid type passed to command from LUA";
      lua_pushnil (L);

      return 1;
    }
  }

  lua_settop (L, 0);

  bool ret;

  if (lua_get_async_context (L)) {
    MarshaledCommand m (*cmd_ptr);
    ret = m.execute (params);
  }
  else
    ret = cmd_ptr->execute (params);

  lua_pushinteger (L, ret);

  return 1;
}



class LUAScriptThread : public QRunnable
{
public:
  LUAScriptThread (const string& script)
    : _script (script)
  {
  }

  void run ()
  {
    lua_start_script (_script, true);
  }

private:
  string _script;
};




bool lua_run_script (const string& script)
{
  return lua_start_script (script, false);
}

void lua_run_script_async (const string& script)
{
  LUAScriptThread* t = new LUAScriptThread (script);
  QThreadPool::globalInstance ()->start (t);
}
