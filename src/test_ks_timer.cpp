#include <iostream>
#include <string>
#include <memory>

#include "ks_timer.h"

using namespace std;

void EchoFunc(string&& s) {
	cout << "test : " << s << endl;
}

int main() {
	KsTimer t;
	// 周期性执行定时任务
	t.StartTimer(1000, bind(EchoFunc, "hello world!"));
	this_thread::sleep_for(chrono::seconds(4));
	cout << "try to expire timer!" << endl;
	t.Expire();

	// 周期性执行定时任务
	t.StartTimer(1000, bind(EchoFunc, "hello c++11"));
	this_thread::sleep_for(chrono::seconds(4));
	cout << "try to expire timer!" << endl;
	t.Expire();

	// 只执行一次定时任务
	//
	// 同步
	t.SyncWait(1000, EchoFunc, "hello world");
	// 异步
	t.AsyncWait(1000, EchoFunc, "hello c++11");

	this_thread::sleep_for(chrono::seconds(2));
	return 0;
}
