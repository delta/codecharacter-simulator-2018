#include "drivers/timer.h"
#include "gtest/gtest.h"

using namespace drivers;
using namespace std;

TEST(TimerTest, All) {
    Timer t;
    bool flag = false;
    int timer_duration = 1000;
    int grace_period = 500;
    t.Start(
        (Timer::Interval(timer_duration)),
        [&flag] { flag = true; }
    );
    this_thread::sleep_for(chrono::milliseconds(timer_duration + grace_period));
    ASSERT_EQ(flag, true);
}
