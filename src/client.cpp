#include "load_run.h"
#include "ks_kinesis.h"

using namespace std;

//fileFilterType defaultIsFilter = [](const char*, const char *) {return true;};

int main() {
	Aws::SDKOptions options;
	Aws::InitAPI(options);
	
	KsKinesis ks("gift", "ap-southeast-1");
	ks.KsStreamDescription();

	KsScripts ksc;

	// 加载lua脚本
	ksc.LoadLuaScript("lua/");

	for (auto script : ksc.GetLuaScripts()) {
		if (!ksc.InitLuaScript(script)) {	// 初始化脚本失败
			cout << "InitLuaScript err, script name: " << script << endl;
			continue;
		}
		if (!ksc.CallLuaScript()) {
			cout << "pcall error, script name: " << script << endl;
			continue;
		}
		for (auto rs : ks.KsStreamDataPull()) {
			ByteBuffer b = rs.GetData();
			string parameterString = ks.ByteBufferToString(b);
			// 通过脚本调用函数
			if (!ksc.Process(parameterString)) {
				cout << "Process err, script name: " << script << endl;
				continue;
			}
		}
	}

	Aws::ShutdownAPI(options);
	return 0;
}
