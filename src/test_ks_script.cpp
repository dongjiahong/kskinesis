#include "ks_script.h"
#include <string>
#include <thread>

using namespace std;

string log = R"({"age": 3})";

void AsyncCall(void *w, function<void(void *)> task) {
	thread t(task, w);
	t.detach();
}

void fun(KsScripts *ks) {
	for (int i=0;i<10;i++) {
		ks->Process(log);
	}
}

int main() {
	string script = "lua/test.lua";
	KsScripts ksc(script);
	if (!ksc.InitLuaScript(script)){
		cout << "InitLuaScript err" << endl;
	}
	//thread t(fun, &ksc);
	AsyncCall(&ksc, bind(fun, &ksc));

	this_thread::sleep_for(chrono::milliseconds(5000));
	return 0;
}
