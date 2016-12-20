#include <functional>
#include <chrono>

#include "ks_script.h"
#include "ks_kinesis.h"

#include "tools.h"

using namespace std;

function<bool(const char*, const char*)> isFilter = [](const char*, const char*) {return true;};

static vector<string> luaScripts = tools::ForEachFile("lua/", isFilter, false);

void ScriptProcess(KsScripts *pksc, KsKinesis *pks) {
	while(true) {
		{
			unique_lock<mutex> locker(Base::dataMutex);
			Base::dataCon.wait(locker, []{return Base::dataReady;}); // 等待数据准备好
			++Base::runDataThreadNum; // 增加消费线程计数
			cout << pksc->GetScriptName() << ":add runDataThreadNum: "<< Base::runDataThreadNum << endl;
		}

		for (auto rs : pks->KsGetDataRecords()) {
			ByteBuffer b = rs.GetData();
			string log = pks->ByteBufferToString(b);

			cout << "call process.. " << pksc->GetScriptName() << endl;
			if (!pksc->Process(log)) {
				continue;
			}
		}

		{
			// 数据处理完成 
			unique_lock<mutex> locker(Base::dataMutex);
			--Base::runDataThreadNum;
			Base::dataProcess = true;
			cout << pksc->GetScriptName() << ":dec runDataThreadNum: "<< Base::runDataThreadNum << endl;
		}
		Base::dataCon.notify_one();
		this_thread::sleep_for(chrono::milliseconds(1000));
	}
}

int main() {
	Aws::SDKOptions options;
	Aws::InitAPI(options);
	
	vector<thread> threads;

	KsKinesis ks("gift", "ap-southeast-1");
	cout << "1. KsKinesis description" << endl;

	ks.KsStreamDescription();

	cout << "2. AsyncCall data pull" << endl;
	tools::AsyncCall(bind(&KsKinesis::KsStreamDataPull, &ks)); // 起数据数据线程

	for (auto script : luaScripts) {
		KsScripts *pksc = new KsScripts(script); // 创建一个实例
		cout << "create a KsScripts instance" << endl;
		if (!pksc->InitLuaScript(script)) {	// 初始化脚本失败
			cout << "InitLuaScript err, script name: " << script << endl;
			continue;
		}
		
		cout << "start up script: " << script << endl;
		//tools::AsyncCallWithWR(&ksc, &ks, bind(ScriptProcess, placeholders::_1, placeholders::_2));
		//tools::AsyncCallWithWR(&ksc, &ks, bind(ScriptProcess, &ksc, &ks));
		threads.push_back(thread(ScriptProcess, pksc, &ks));
		//thread t(ScriptProcess, &ksc, &ks);
		//threads.push_back(t);
	}

	for (auto &th : threads) {
		th.join();
	}

	//this_thread::sleep_for(chrono::minutes(1));

	Aws::ShutdownAPI(options);
	return 0;
}
