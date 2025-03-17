#include "WatchdogTimerTest.h"
#include "WatchdogTimer.h"
#include "Board.h"

const char* WatchdogTimerTest::NAME = "WatchdogTimerTest";

const char* WatchdogTimerTest::getName() const {
    return WatchdogTimerTest::NAME;
}

uint8_t WatchdogTimerTest::runTests(void (*log)(const char* format, ...)) const {
    WatchdogTimer& watchdogTimer = Board::get().getWatchdogTimer();

    watchdogTimer.sleep(1000, WatchdogTimer::SleepMode::IDLE);

    return 0;
}
