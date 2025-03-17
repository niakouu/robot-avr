#ifndef _TIMER_FREQUENCY_TEST_H
#define _TIMER_FREQUENCY_TEST_H

#include "Test.h"

class TimerFrequencyTest : public Test {
public:
    TimerFrequencyTest() = default;

    const char* getName() const override;
    uint8_t runTests(void (*log)(const char* format, ...)) const override;

private:
    static const char* NAME;
};

#endif /* _TIMER_FREQUENCY_TEST_H */