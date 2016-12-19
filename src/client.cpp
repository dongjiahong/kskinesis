#include <functional>
#include <chrono>

#include "load_run.h"
#include "ks_kinesis.h"
#include "tools.h"

using namespace std;

function<bool(const char*, const char*)> isFilter = [](const char*, const char*) {return true;};

void ScriptProcess(KsScripts *pksc, KsKinesis *pks) {
	cout << "run ScriptProcess " << endl;
	{
		unique_lock<mutex> locker(tools::dataMutex);
		tools::dataCon.wait(locker, []{return tools::dataReady;}); // 等待数据准备好
		++tools::runDataThreadNum; // 增加消费线程计数
		cout << "add runDataThreadNum: "<< tools::runDataThreadNum << endl;
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
		cout << "dec runDataThreadNum: "<< tools::runDataThreadNum << endl;
	}
}

int main() {
	Aws::SDKOptions options;
	Aws::InitAPI(options);
	
	KsKinesis ks("gift", "ap-southeast-1");
	cout << "1. KsKinesis description" << endl;

	ks.KsStreamDescription();

	cout << "2. AsyncCall data pull" << endl;
	tools::AsyncCall(bind(&KsKinesis::KsStreamDataPull, &ks)); // 起数据数据线程

	// 加载lua脚本
	cout << "3. load lua script" << endl;
	//tools::LoadLuaScript("lua/");
	tools::LoadLuaScript("/Users/lele/myGit/kiss_kinesis/lua/");

	cout << "4. script size: " << tools::luaScripts.size() << endl;

	for (auto script : tools::ForEachFile("/Users/lele/myGit/kiss_kinesis/lua/", isFilter, false)) {
	//for (auto script : tools::GetLuaScripts()) {
		KsScripts ksc; // 创建一个实例
		cout << "create a KsScripts instance" << endl;
		if (!ksc.InitLuaScript(script)) {	// 初始化脚本失败
			cout << "InitLuaScript err, script name: " << script << endl;
			continue;
		}
		if (!ksc.CallLuaScript()) {
			cout << "pcall error, script name: " << script << endl;
			continue;
		}
		
		cout << "start up script: " << script << endl;
		//tools::AsyncCallWithWR(&ksc, &ks, bind(ScriptProcess, placeholders::_1, placeholders::_2));
		tools::AsyncCallWithWR(&ksc, &ks, bind(ScriptProcess, &ksc, &ks));
	}

	cout << "sleep 50 seconds" << endl;
	this_thread::sleep_for(chrono::milliseconds(50000));

	Aws::ShutdownAPI(options);
	return 0;
}
