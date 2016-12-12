
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

using namespace std;

using fileFilterType = function<bool(const char *, const char *)>;

class KsScripts {

public:
	KsScripts() {
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


	// 循环遍历文件夹
	vector<string> ForEachFile(const string &dirName, fileFilterType filter, bool sub = false);
	// 加载lua脚本
	void LoadLuaScript(const string &scriptPath);

	// get luaEnv
	lua_State* GetLuaEnv() { return luaEnv; }
	// 初始化脚本文件
	bool InitLuaScript(const string luaScript) {
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

	bool CallLuaScript() {
		int bRet = lua_pcall(luaEnv, 0, 0, 0);
		if (bRet) {
			cout << "pcall error" << endl;
			PrintStackTopError();
			return false;
		}
		return true;
	}

	// 执行脚本函数process
	bool Process(const string &log) {
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

	vector<string>& GetLuaScripts() {
		return luaScripts;
	}

protected:

private:
	// 判断是否是文件夹
	bool IsFolder(const char* dirName);
	bool IsFolder(const string &dirName);
	// 字符串转小写
	inline string toLower(const string &src) {
		auto dst = src;
		transform(src.begin(), src.end(), dst.begin(), ::tolower);
		return dst;
	}
	// 判断文件是否以指定字符串结尾
	inline bool EndWith(const string &src, const string &suffix) {
		return src.substr(src.size() - suffix.size()) == suffix;
	} 
	// 打印栈顶的错误
	void PrintStackTopError() {
		if (luaEnv != nullptr) {
			const char *pErrorMsg = lua_tostring(luaEnv, -1); // -1 表示栈顶
			cout << " Scripts::PrintStackError " << pErrorMsg << endl;
		} else {
			cout << "no initialize lua env!" << endl;
		}
	}


	lua_State* luaEnv;
	vector<string> luaScripts;
};
