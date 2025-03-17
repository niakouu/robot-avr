#ifndef _MOVEMENTMANAGER_TEST_H
#define _MOVEMENTMANAGER_TEST_H

#include "Test.h"

class MovementManagerTest : public Test {
public:
    MovementManagerTest() = default;

    const char* getName() const override;
    uint8_t runTests(void (*log)(const char* format, ...)) const override;

private:
    static const char* NAME;
};

#endif /* _MOVEMENTMANAGER_TEST_H */