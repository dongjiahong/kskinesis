#include "ks_timer.h"

using namespace std;

void KsTimer::StartTimer(int interval, function<void()> task) {
	if (!m_expired) {
		cout << "timer is currently running, please expire it first..." << endl;
		return;
	}

	m_expired = false;

	// 捕获当前类的this指针，让lambda函数拥有和当前成员函数一样的访问权限。
	thread([this, interval, task]() {
			while (!m_tryToExpire) {
			// this_thread this namespace groups a set of functions that access the current thread
				this_thread::sleep_for(chrono::milliseconds(interval));
				task();
			}

			{
				lock_guard<mutex> locker(m_mutex);
				m_expired = true;
				m_expiredCond.notify_one();
			}

			}).detach();
}

void KsTimer::Expire() {
	if (m_expired) {
		return;
	}

	if (m_tryToExpire) {
		cout << "timer is trying to expire, please wait..." << endl;
		return;
	}

	m_tryToExpire = true;

	{
		unique_lock<mutex> locker(m_mutex);
		m_expiredCond.wait(locker, [this]{return m_expired == true;});

		if (m_expired) {
			cout << "timer expired!" << endl;
			m_tryToExpire = false;
		}
	}
}
