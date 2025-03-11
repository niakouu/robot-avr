#ifndef _LED_TEST_H
#define _LED_TEST_H

#include "Test.h"

class LedTest : public Test {
public:
    LedTest() = default;

    const char* getName() const override;
    uint8_t runTests(void (*log)(const char* name, const char* format,
                                 ...)) const override;

private:
    static const char* NAME;
};

#endif /* _LED_TEST_H */