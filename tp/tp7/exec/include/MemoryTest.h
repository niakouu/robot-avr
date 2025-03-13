#ifndef _MEMORY_TEST_H
#define _MEMORY_TEST_H

#include "Test.h"

class MemoryTest : public Test {
public:
    MemoryTest() = default;

    const char* getName() const override;
    uint8_t runTests(void (*log)(const char* format, ...)) const override;

private:
    static const char* NAME;
};

#endif /* _MEMORY_TEST_H */