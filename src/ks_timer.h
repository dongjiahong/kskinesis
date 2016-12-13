#ifndef _KS_TIMER_H_
#define _KS_TIMER_H_

#include <iostream>
#include <functional>
#include <chrono>
#include <thread>
#include <atomic>
#include <memory>
#include <mutex>
#include <condition_variable>

using namespace std;

class KsTimer {
public:
	KsTimer() : m_expired(true), m_tryToExpire(false){}

	KsTimer(const KsTimer& t) {
		m_expired = t.m_expired.load(); // load 是取出数据, 对应的save是写入数据
		m_tryToExpire = t.m_tryToExpire.load();
	}

	~KsTimer() {
		Expire();
	}

	void StartTimer(int interval, function<void()> task); 

	void Expire();

	// 一次任务同步执行
	template<typename callable, class... arguments>
	void SyncWait(int after, callable&& f, arguments&&... args) {

		// forward<T>(u) 有两个参数：T与u。
		// 当T为左值引用类型时，u将被转换为T类型的左值，否则将被转换为T类型的右值。
		// 如此定义std::forward是为了在使用右值引用参数的函数模板中解决参数的完美转发问题。
		function<typename result_of<callable(arguments...)>::type()> task
			(bind(forward<callable>(f), forward<arguments>(args)...));

		this_thread::sleep_for(chrono::milliseconds(after));
		task();
	}

	// 一次任务异步执行
	template<typename callable, class... arguments>
	void AsyncWait(int after, callable&& f, arguments&&... args) {

		function<typename result_of<callable(arguments...)>::type()> task
			(bind(forward<callable>(f), forward<arguments>(args)...));

		thread([after, task]() {
				this_thread::sleep_for(chrono::milliseconds(after));
				task();
		}).detach();
	}

private:
	atomic<bool> m_expired;	
	atomic<bool> m_tryToExpire;

	mutex m_mutex;
	condition_variable m_expiredCond;
};
#endif
