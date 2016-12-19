#ifndef _BASE_H_
#define _BASE_H_

#include <mutex>
#include <condition_variable>
#include <string>

using namespace std;

class Base {
public:
	// 公共使用的一些变量和操作
	// 静态变量,需要显示初始化，不然会有编译reference错误
	static mutex dataMutex;
	static condition_variable dataCon;
	static bool dataReady;
	static bool dataProcess;
	static int runDataThreadNum;

};

#endif
