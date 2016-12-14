#include <iostream>
#include <functional>
#include <condition_variable>
#include <mutex>
#include <vector>
#include <dirent.h>

using namespace std;

namespace tools{

mutex dataMutex;
condition_variable dataCon;
bool dataReady = false;
bool dataProcess = false;
int runDataThreadNum = 0; // 数据处理线程的全局标识

vector<string> luaScripts;
using fileFilterType = function<bool(const char *, const char *)>;

void AsyncCallWithInterval(int interval, std::function<void()> task);
void AsyncCall(std::function<void()> task);
void AsyncCallWithWR(std::function<void(void *, void *)> task);
//void AsyncCallWithWR(std::function<void(void *, void *)> task, void *w, void *r);

// 加载lua脚本
void LoadLuaScript(const string &scriptPath);
// 循环遍历文件夹
vector<string> ForEachFile(const string &dirName, fileFilterType filter, bool sub = false);

vector<string>& GetLuaScripts() {return luaScripts;}
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

}
