#include "drivers/timer.h"

namespace drivers {

bool Timer::Start(
	Interval total_timer_duration,
	const Callback& callback)
{
	if (this->is_running) {
		return false;
	}

	std::thread([this](Interval total_timer_duration, const Callback& callback){
		std::this_thread::sleep_for(total_timer_duration);
		this->is_running = false;
		callback();
	}, total_timer_duration, std::ref(callback)).detach();

	return true;
}

}
