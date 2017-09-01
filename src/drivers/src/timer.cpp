#include "drivers/timer.h"

namespace drivers {

Timer::Timer() : is_running(false) {}

bool Timer::Start(
	Interval total_timer_duration,
	const Callback& callback)
{
	if (this->is_running) {
		return false;
	}

	this->is_running = true;

	std::thread([this](Interval total_timer_duration, const Callback& callback){
		std::this_thread::sleep_for(total_timer_duration);
		this->is_running = false;
		callback();
	}, total_timer_duration, std::ref(callback)).detach();

	return true;
}

}
