#ifndef _BUTTON_TEST_H
#define _BUTTON_TEST_H

#include "Test.h"

class ButtonTest : public Test {
public:
    ButtonTest() = default;

    const char* getName() const override;
    uint8_t runTests(void (*log)(const char* format, ...)) const override;

private:
    static const char* NAME;
};

#endif /* _BUTTON_TEST_H */