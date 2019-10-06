#include "Hooks.h"
#include "jmphookedfunction.h"
#include <cstdint>
#include "Lua/lua/lua.h"


static FILE* fdebug = 0;



typedef decltype(&lua_pcall) lua_pcall_type;
static lua_pcall_type real_lua_pcall = (lua_pcall_type)0xA5D8D0;
static JmpHookedFunction<lua_pcall_type>* lua_pcall_hook;

void InstallHooks()
{
    if (!fdebug)
    {
		fdebug = fopen("hooked.txt", "wb");
    }

	lua_pcall_hook = new JmpHookedFunction<lua_pcall_type>(real_lua_pcall, &lua_pcall);
}
