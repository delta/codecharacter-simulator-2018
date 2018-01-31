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

	/**
	 * Temporary used by the Cancel method
	 *
	 * Set to true by Cancel, and reset to false right before Cancel returns
	 */
	std::atomic_bool cancel;

	/**
	 * Amount of time the timer has been running for
	 *
	 * Valid if is_running is true
	 */
	std::chrono::milliseconds cur_duration;

	/**
	 * Timer wakes up every timer_wake_up_duration ms to check if it has been
	 * cancelled
	 */
	std::chrono::milliseconds timer_wake_up_duration;

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

	/**
	 * Method to cancel the timer
	 *
	 * Cancelled timer won't call callback unless it's very close to expiring
	 * already.
	 * Blocks for at most timer_wake_up_duration ms
	 */
	void Cancel();
};
}

#endif
