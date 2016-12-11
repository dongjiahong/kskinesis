#include "load_run.h"

using namespace std;

fileFilterType defaultIsFilter = [](const char *, const char *){return true;};

//vector<string> Is(const string& dirName, bool sub = false) {
	//return ForEachFile(dirName, defaultIsFilter, sub);
//}

int main() {
	Scripts sc;

	for (auto file : sc.ForEachFile("./", defaultIsFilter, false)) {
		cout << file << endl;
	}
}
