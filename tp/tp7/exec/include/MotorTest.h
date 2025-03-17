#ifndef _MOTOR_TEST_H
#define _MOTOR_TEST_H

#include "Test.h"

class MotorTest : public Test {
public:
    MotorTest() = default;

    const char* getName() const override;
    uint8_t runTests(void (*log)(const char* format, ...)) const override;

private:
    static const char* NAME;
};

#endif /* _MOTOR_TEST_H */