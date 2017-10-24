#ifndef DRIVERS_SPIN_LOCK_H
#define DRIVERS_SPIN_LOCK_H

namespace drivers {

class SpinLock {
	std::atomic_flag m_flag = ATOMIC_FLAG_INIT;

	public:
		void Lock()     noexcept {   while(m_flag.test_and_set(std::memory_order_relaxed)); }
		void Unlock()   noexcept {         m_flag.clear(std::memory_order_relaxed);         }
		bool TryLock() noexcept { return !m_flag.test_and_set(std::memory_order_relaxed);  }
};

}

#endif
