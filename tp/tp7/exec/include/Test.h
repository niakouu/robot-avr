#ifndef _TEST_H
#define _TEST_H

#include <stdint.h>

class Test {
public:
    virtual const char* getName() const = 0;
    virtual uint8_t runTests(void (*log)(const char* name, const char* format,
                                         ...)) const = 0;
};

#endif /* _TEST_H */