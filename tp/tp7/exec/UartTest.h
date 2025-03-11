#ifndef _UART_TEST_H
#define _UART_TEST_H

#include "Test.h"

class UartTest : public Test {
public:
    UartTest() = default;

    const char* getName() const override;
    uint8_t runTests(void (*log)(const char* name, const char* format,
                                 ...)) const override;

private:
    static const char* NAME;
};

#endif /* _UART_TEST_H */