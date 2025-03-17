#include "WatchdogTimerTest.h"
#include "WatchdogTimer.h"
#include "Board.h"

const char* WatchdogTimerTest::NAME = "WatchdogTimerTest";
constexpr uint16_t MILLISECONDS_IN_SECONDS = 1000;

const char* WatchdogTimerTest::getName() const {
    return WatchdogTimerTest::NAME;
}

uint8_t WatchdogTimerTest::runTests(void (*log)(const char* format, ...)) const {
    WatchdogTimer& watchdogTimer = Board::get().getWatchdogTimer();

    watchdogTimer.sleep(MILLISECONDS_IN_SECONDS, WatchdogTimer::SleepMode::IDLE);

    return 0;
}
