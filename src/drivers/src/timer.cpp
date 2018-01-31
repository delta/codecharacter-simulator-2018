#include "drivers/timer.h"

namespace drivers {

Timer::Timer()
    : is_running(false), cancel(false), cur_duration(0),
      timer_wake_up_duration(10) {}

bool Timer::Start(Interval total_timer_duration, Callback callback) {
	if (this->is_running) {
		return false;
	}

	this->is_running = true;
	this->cur_duration = Interval(0);

	std::thread(
	    [this](Interval total_timer_duration, Callback callback) {
		    while (this->cur_duration < total_timer_duration && !this->cancel) {
			    std::this_thread::sleep_for(this->timer_wake_up_duration);
			    this->cur_duration += this->timer_wake_up_duration;
		    }
		    this->is_running = false;
		    if (!this->cancel)
			    callback();
		},
	    total_timer_duration, std::ref(callback))
	    .detach();

	return true;
}

void Timer::Cancel() {
	this->cancel = true;
	while (this->is_running)
		;
	this->cancel = false;
}
}
