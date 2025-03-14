#ifndef _PHOTORESISTANCE_TEST_H
#define _PHOTORESISTANCE_TEST_H

#include "Test.h"

class PhotoresistanceTest : public Test {
public:
    PhotoresistanceTest() = default;

    const char* getName() const override;
    uint8_t runTests(void (*log)(const char* format, ...)) const override;

private:
    static const char* NAME;
};

#endif /* _PHOTORESISTANCE_TEST_H */