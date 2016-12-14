#include <functional>
#include <chrono>

#include "load_run.h"
#include "ks_kinesis.h"
#include "tools.h"

using namespace std;


//fileFilterType defaultIsFilter = [](const char*, const char *) {return true;};

void ScriptProcess(KsScripts *pksc, KsKinesis *pks) {
	{
		unique_lock<mutex> locker(tools::dataMutex);
		tools::dataCon.wait(locker, []{return tools::dataReady;}); // 等待数据准备好
		++tools::runDataThreadNum; // 增加消费线程计数
	}

	for (auto rs : pks->KsGetDataRecords()) {
		ByteBuffer b = rs.GetData();
		string log = pks->ByteBufferToString(b);
		if (!pksc->Process(log)) {
			continue;
		}
	}

	{
		// 数据处理完成 
		lock_guard<mutex> locker(tools::dataMutex);
		--tools::runDataThreadNum;
		tools::dataProcess = true;
	}
}

int main() {
	Aws::SDKOptions options;
	Aws::InitAPI(options);
	
	KsKinesis ks("gift", "ap-southeast-1");
	ks.KsStreamDescription();

	tools::AsyncCall(bind(&KsKinesis::KsStreamDataPull, &ks)); // 起数据数据线程

	// 加载lua脚本
	tools::LoadLuaScript("lua/");

	for (auto script : tools::GetLuaScripts()) {
		KsScripts ksc; // 创建一个实例
		if (!ksc.InitLuaScript(script)) {	// 初始化脚本失败
			cout << "InitLuaScript err, script name: " << script << endl;
			continue;
		}
		if (!ksc.CallLuaScript()) {
			cout << "pcall error, script name: " << script << endl;
			continue;
		}
		
		cout << "start up script: " << script << endl;
		tools::AsyncCallWithWR(bind(ScriptProcess, &ksc, &ks));
		//tools::AsyncCallWithWR(bind(ScriptProcess, placeholders::_1, placeholders::_2), &ksc, &ks);
	}
	this_thread::sleep_for(chrono::milliseconds(1000));

	Aws::ShutdownAPI(options);
	return 0;
}
