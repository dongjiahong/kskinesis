#ifndef _KS_SCRIPT_H_
#define _KS_SCRIPT_H_

extern "C" {
    #include "lua.h"
    #include "lauxlib.h"
    #include "lualib.h"
	#pragma comment(lib, "lua.lib")
}

#include <iostream>
#include <string.h>
#include <functional>
#include <algorithm>
#include <vector>
#include <dirent.h>

#include "base.h"

using namespace std;

class KsScripts : public Base {

public:
	KsScripts(const string &name) : scriptName(name) {
		// 创建lua环境
		luaEnv = luaL_newstate();

		// 加载lua基础库
		luaopen_base(luaEnv);

		// 加载lua高级库： io、string、table等
		luaL_openlibs(luaEnv);
	}

	~KsScripts() {
		if (luaEnv != nullptr) {
			lua_close(luaEnv);
		}
	}

	// get luaEnv
	lua_State* GetLuaEnv() { return luaEnv; }
	// 初始化脚本文件
	bool InitLuaScript(const string luaScript);
	// 执行脚本函数process
	bool Process(const string &log);
	string GetScriptName() {return scriptName;}

	// ----------文件操作

protected:

private:
	// 打印栈顶的错误
	void PrintStackTopError() {
		if (luaEnv != nullptr) {
			const char *pErrorMsg = lua_tostring(luaEnv, -1); // -1 表示栈顶
			cout << "Scripts::PrintStackError " << pErrorMsg << endl;
		} else {
			cout << "no initialize lua env!" << endl;
		}
	}

	lua_State* luaEnv;
	string scriptName;
};

#endif
