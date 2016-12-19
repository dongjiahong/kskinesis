#include "base.h"

// 显示初始化静态变量
bool Base::dataReady = false;
bool Base::dataProcess = false;

int Base::runDataThreadNum = 0;

mutex Base::dataMutex;

condition_variable Base::dataCon;
