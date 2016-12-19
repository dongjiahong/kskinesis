#include "load_run.h"
#include "tools.h"

using namespace std;
using namespace tools;

tools::fileFilterType defaultIsFilterx = [](const char *, const char *){return true;};

//vector<string> Is(const string& dirName, bool sub = false) {
	//return ForEachFile(dirName, defaultIsFilter, sub);
//}

int main() {
	KsScripts sc;

	//for (auto file : tools::ForEachFile("./", defaultIsFilterx, false)) {
		//cout << file << endl;
	//}
}
