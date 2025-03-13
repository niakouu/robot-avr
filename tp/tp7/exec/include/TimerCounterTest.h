#ifndef _TIMER_COUNTER_TEST_H
#define _TIMER_COUNTER_TEST_H

#include "Test.h"

class TimerCounterTest : public Test {
public:
    TimerCounterTest() = default;

    const char* getName() const override;
    uint8_t runTests(void (*log)(const char* format, ...)) const override;

private:
    static const char* NAME;
};

#endif /* _TIMER_COUNTER_H */