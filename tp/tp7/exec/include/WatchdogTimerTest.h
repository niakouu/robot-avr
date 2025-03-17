#ifndef _WATCHDOG_TIMER_TEST_H
#define _WATCHDOG_TIMER_TEST_H

#include "Test.h"

class WatchdogTimerTest : public Test {
public:
    WatchdogTimerTest() = default;

    const char* getName() const override;
    uint8_t runTests(void (*log)(const char* format, ...)) const override;

private:
    static const char* NAME;
};

#endif /* _WATCHDOG_TIMER_TEST_H */