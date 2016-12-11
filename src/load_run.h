
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

class Scripts {

public:
	Scripts() {
		// 创建lua环境
		luaEnv = luaL_newstate();

		// 加载lua基础库
		luaopen_base(luaEnv);

		// 加载lua高级库： io、string、table等
		luaL_openlibs(luaEnv);
	}

	~Scripts() {
		if (luaEnv != nullptr) {
			lua_close(luaEnv);
		}
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

	// 循环遍历文件夹
	vector<string> ForEachFile(const string &dirName, fileFilterType filter, bool sub = false);

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

	lua_State* luaEnv;

	vector<string> luaScripts;
};
