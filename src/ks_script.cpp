
#include <iostream>
#include <string.h>

#include "ks_script.h"
#include "tools.h"

using namespace std;

// 执行脚本函数process
bool KsScripts::Process(const string &log) {
	string name = GetScriptName();
	if (luaEnv == nullptr) {
		cout << name << " luaEnv is nullptr" << endl;
		return false;
	}

	lua_getglobal(luaEnv, "handle_log");

	lua_pushstring(luaEnv, log.c_str());

	int iRet = lua_pcall(luaEnv, 1, 1, 0);
	if (iRet) {
		PrintStackTopError();
		return false;
	}

	if (lua_isnumber(luaEnv, -1)) {
		double v = lua_tonumber(luaEnv, -1);
		cout << name << " the result is :" << v << endl;
	}

	return true;
}

bool KsScripts::InitLuaScript(const string luaScript) {
	if (luaEnv == nullptr) {
		PrintStackTopError();
		return false;
	}
	//int bRet = luaL_loadfile(luaEnv, luaScript.c_str()); // 只是加载没编译
	//luaL_pcall(luaEnv, 0, 0, 0); // 编译
	//luaL_dofile 等同于上面两步，既加载并编译
	int bRet = luaL_dofile(luaEnv, luaScript.c_str());
	if (bRet) {
		PrintStackTopError();
        return false;
	}
	return true;
}

