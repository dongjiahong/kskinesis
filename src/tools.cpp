#include <iostream>
#include <thread>
#include <chrono>
#include <string.h>

#include "tools.h"
#include <memory>

using namespace std;
//using namespace tools;

//tools::fileFilterType defaultIsFilter = [](const char*, const char*) {return true;};

void tools::AsyncCallWithInterval(int interval, function<void()> task) {
	thread([interval, task] {
		while (true) {
			this_thread::sleep_for(chrono::milliseconds(interval));
			task();
		}	
	}).detach();
}

void tools::AsyncCall(function<void()> task) {
	//thread([task] {
		//task();
	//}).detach();
	thread t(task);
	t.detach();
}

void tools::AsyncCallWithWR(void *w, void *r, function<void(void *, void *)> task) {
	//thread([task] {
		//task();
	//}).detach();
	cout << "AsyncCallWithWR .." << endl;
	thread t(task, w, r);
	t.detach();
}

// 判断是否是文件夹
bool tools::IsFolder(const char* dirName) {
	if ( dirName == nullptr) {
		cout << "IsFolder:: dirName is nullptr";
		return false;
	}

	auto dir = opendir(dirName);
	if (dir) {
		closedir(dir);
		return true;
	}
	return false;
}

// 判断是否是文件夹
bool tools::IsFolder(const string &dirName) {
	if (dirName.empty()) {
		cout << "IsFoler:: dirName is empty" << endl;
		return false;
	}
	return IsFolder(dirName.data());
}

vector<string> tools::ForEachFile(const string &dirName, fileFilterType filter, bool sub) {
	vector<string> v;
	auto dir = opendir(dirName.data());
	struct dirent *ent;
	if (dir) {
		while ((ent = readdir(dir)) != nullptr) {
			//auto p = string(dirName).append("/").append(ent->d_name);
			auto p = string(dirName).append(ent->d_name);
			if (sub) {
				if (0 == strcmp(ent->d_name, "..") || 0 == strcmp(ent->d_name, ".")) {
					continue;
				} else if (IsFolder(p)) {
					auto r = ForEachFile(p, filter, sub);
					v.insert(v.end(), r.begin(), r.end());
					continue;
				}
			}
			if (sub || !IsFolder(p)) { // 如果是文件， 则调用文件过滤器
				if (filter(dirName.data(), ent->d_name)) {
					v.emplace_back(p);
				}
			}
		}
		closedir(dir);
	}
	return v;
}

void tools::LoadLuaScript(const string &scriptPath) {
	if (scriptPath.size() == 0) {
		cout << " scriptPath is null!" << endl;
		return;
	}
    //for (auto script : ForEachFile(scriptPath, defaultIsFilter, false)) {
    {
	//	luaScripts.push_back(script);
	}
    cout << "toos::luaScripts size:" << tools::luaScripts.size()<< endl;
}
