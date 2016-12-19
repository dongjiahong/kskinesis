#ifndef _KS_TOOLS_H_
#define _KS_TOOLS_H_

#include <iostream>
#include <functional>
#include <algorithm>
#include <condition_variable>
#include <mutex>
#include <vector>
#include <dirent.h>

using namespace std;

namespace tools{

static mutex dataMutex;
static condition_variable dataCon;
static bool dataReady = false;
static bool dataProcess = false;
static int runDataThreadNum = 0; // 数据处理线程的全局标识

static vector<string> luaScripts;

using fileFilterType = function<bool(const char *, const char *)>;

void AsyncCallWithInterval(int interval, std::function<void()> task);
void AsyncCall(std::function<void()> task);
void AsyncCallWithWR(void *w, void *r, std::function<void(void *, void *)> task);

// 循环遍历文件夹
vector<string> ForEachFile(const string &dirName, fileFilterType filter, bool sub = false);

// 加载lua脚本
void LoadLuaScript(const string &scriptPath);

static vector<string> GetLuaScripts() {
	cout << "GetLuaScripts size:" << luaScripts.size() << endl;
	return luaScripts;
}
// 字符串转小写
inline string toLower(const string &src) {
	auto dst = src;
	transform(src.begin(), src.end(), dst.begin(), ::tolower);
	return dst;
}

bool IsFolder(const char *);
bool IsFolder(const string &);
// 判断文件是否以指定字符串结尾
inline bool EndWith(const string &src, const string &suffix) {
	return src.substr(src.size() - suffix.size()) == suffix;
} 

}
#endif
