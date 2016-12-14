
#include <iostream>
#include <string.h>

#include "load_run.h"

using namespace std;

// 执行脚本函数process
bool KsScripts::Process(const string &log) {
	lua_getglobal(luaEnv, "process");
	lua_pushstring(luaEnv, log.c_str());
	int iRet = lua_pcall(luaEnv, 1, 1, 0);
	if (iRet) {
		PrintStackTopError();
		return false;
	}

	if (lua_isnumber(luaEnv, -1)) {
		double v = lua_tonumber(luaEnv, -1);
		cout << " the result is :" << v << endl;
	}

	return true;
}

bool KsScripts::InitLuaScript(const string luaScript) {
	if (luaEnv == nullptr) {
		PrintStackTopError();
		return false;
	}
	int bRet = luaL_loadfile(luaEnv, luaScript.c_str());
	if (bRet) {
		PrintStackTopError();
	}
	return true;
}

bool KsScripts::CallLuaScript() {
	int bRet = lua_pcall(luaEnv, 0, 0, 0);
	if (bRet) {
		cout << "pcall error" << endl;
		PrintStackTopError();
		return false;
	}
	return true;
}
