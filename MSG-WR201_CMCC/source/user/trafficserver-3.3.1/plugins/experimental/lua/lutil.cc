/*
  Licensed to the Apache Software Foundation (ASF) under one
  or more contributor license agreements.  See the NOTICE file
  distributed with this work for additional information
  regarding copyright ownership.  The ASF licenses this file
  to you under the Apache License, Version 2.0 (the
  "License"); you may not use this file except in compliance
  with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/

#include <ts/ts.h>
#include <ts/remap.h>
#include "lapi.h"
#include "lutil.h"
#include <pthread.h>
#include <unistd.h>

static thread_local_pointer<LuaThreadInstance> LuaThread;

LuaPluginState * LuaPlugin;
int LuaHttpArgIndex;

LuaThreadInstance::LuaThreadInstance()
  : lua(NULL)
{
  for (unsigned i = 0; i < countof(this->hooks); ++i) {
    this->hooks[i] = LUA_NOREF;
  }
}

LuaThreadInstance::~LuaThreadInstance()
{
}

#if !defined(LUAJIT_VERSION)
static void *
LuaAllocate(void * ud, void * ptr, size_t osize, size_t nsize)
{
  TSReleaseAssert(ud == NULL);

  if (nsize == 0) {
    TSfree(ptr);
    return NULL;
  }

  return TSrealloc(ptr, nsize);
}
#endif

lua_State *
LuaPluginNewState(void)
{
  lua_State * lua;

  // lua_newstate() is a stub in LuaJIT 64-bit.
#if defined(LUAJIT_VERSION)
  lua = luaL_newstate();
#else
  lua = lua_newstate(LuaAllocate, NULL);
#endif
  if (lua == NULL) {
    return NULL;
  }

  LuaLoadLibraries(lua);
  LuaRegisterLibrary(lua, "ts", LuaApiInit);
  LuaRegisterLibrary(lua, "ts.config", LuaConfigApiInit);
  LuaRegisterLibrary(lua, "ts.hook", LuaHookApiInit);

  return lua;
}

lua_State *
LuaPluginNewState(LuaPluginState * plugin)
{
  lua_State * lua;

  lua = LuaPluginNewState();
  if (lua == NULL) {
    return NULL;
  }

  if (!LuaPluginLoad(lua, plugin)) {
    lua_close(lua);
    return NULL;
  }

  return lua;
}

bool
LuaPluginLoad(lua_State * lua, LuaPluginState * plugin)
{
  for (LuaPluginState::pathlist::const_iterator p = plugin->paths.begin(); p < plugin->paths.end(); ++p) {
    if (access(p->c_str(), F_OK) != 0) {
      continue;
    }

    if (luaL_dofile(lua, p->c_str()) != 0) {
      // If the load failed, it should have pushed an error message.
      LuaLogError("failed to load Lua file %s: %s", p->c_str(), lua_tostring(lua, -1));
      return false;
    }
  }

  return true;
}

void
LuaPushMetatable(lua_State * lua, const char * name, const luaL_Reg * exports)
{
  luaL_newmetatable(lua, name);
  lua_pushvalue(lua, -1);
  lua_setfield(lua, -2, "__index");
  luaL_register(lua, NULL, exports);
}

void
LuaRegisterLibrary(lua_State * lua, const char * name, lua_CFunction loader)
{
  // Pull up the preload table.
  lua_getglobal(lua, "package");
  lua_getfield(lua, -1, "preload");

  lua_pushcfunction(lua, loader);
  lua_setfield(lua, -2, name);

  // Pop the 'package' and 'preload' tables.
  lua_pop(lua, 2);
}

void
LuaLoadLibraries(lua_State * lua)
{
#define REGISTER_LIBRARY(name) LuaRegisterLibrary(lua, #name, luaopen_ ## name)

    lua_cpcall(lua, luaopen_base, NULL);
    lua_cpcall(lua, luaopen_package, NULL);

    REGISTER_LIBRARY(io);
    REGISTER_LIBRARY(os);
    REGISTER_LIBRARY(table);
    REGISTER_LIBRARY(string);
    REGISTER_LIBRARY(math);
    REGISTER_LIBRARY(debug);

    // XXX LuaJIT recommends calling luaL_openlibs() here. No explanation of why.

#undef REGISTER_LIBRARY
}

void
LuaSetConstantField(lua_State * lua, const char * name, int value)
{
  lua_pushinteger(lua, value);
  lua_setfield(lua, -2, name);
}

void
LuaSetConstantField(lua_State * lua, const char * name, const char * value)
{
  lua_pushstring(lua, value);
  lua_setfield(lua, -2, name);
}

LuaThreadInstance *
LuaGetThreadInstance()
{
  return LuaThread.get();
}

void
LuaSetThreadInstance(LuaThreadInstance * lthread)
{
  LuaThread.set(lthread);
}

