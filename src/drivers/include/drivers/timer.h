/**
 * @file timer.h
 * Declarations for a timer utility
 */
#ifndef DRIVERS_TIMER_H
#define DRIVERS_TIMER_H

#include "drivers/drivers_export.h"
#include <atomic>
#include <chrono>
#include <cstdint>
#include <functional>
#include <thread>

namespace drivers {

/**
 * An asynchronous timer class
 */
class DRIVERS_EXPORT Timer {
  private:
	/**
	 * true if this timer is running, false otherwise
	 */
	std::atomic_bool is_running;

  public:
	/**
	 * Interval of time that timer operates for
	 */
	typedef std::chrono::milliseconds Interval;

	/**
	 * Callback that timer can call
	 */
	typedef std::function<void(void)> Callback;

	/**
	 * Constructor for Timer
	 */
	Timer();

	/**
	 * Starts this timer. Works only if is_running is false.
	 *
	 * Spawns a separate thread for the timer
	 *
	 * @param[in]  total_timer_duration  The total timer duration
	 * @param[in]  callback              The callback when timer expires
	 *
	 * @return     false if is_running is true, else true
	 */
	bool Start(Interval total_timer_duration, Callback callback);
};
}

#endif
