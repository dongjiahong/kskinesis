#include "load_run.h"

using namespace std;

fileFilterType defaultIsFilterx = [](const char *, const char *){return true;};

//vector<string> Is(const string& dirName, bool sub = false) {
	//return ForEachFile(dirName, defaultIsFilter, sub);
//}

int main() {
	KsScripts sc;

	for (auto file : sc.ForEachFile("./", defaultIsFilterx, false)) {
		cout << file << endl;
	}
}
