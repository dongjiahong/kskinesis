
#include <iostream>
#include <string.h>

#include "load_run.h"

using namespace std;

// 判断是否是文件夹
bool Scripts::IsFolder(const char* dirName) {
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
bool Scripts::IsFolder(const string &dirName) {
	if (dirName.empty()) {
		cout << "IsFoler:: dirName is empty" << endl;
		return false;
	}
	return IsFolder(dirName.data());
}

vector<string> Scripts::ForEachFile(const string &dirName, fileFilterType filter, bool sub) {
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
