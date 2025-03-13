#ifndef _TIMER_PWM_TEST_H
#define _TIMER_PWM_TEST_H

#include "Test.h"

class TimerPwmTest : public Test {
public:
    TimerPwmTest() = default;

    const char* getName() const override;
    uint8_t runTests(void (*log)(const char* name, const char* format,
                                 ...)) const override;

private:
    static const char* NAME;
};

#endif /* _TIMER_PWM_TEST_H */