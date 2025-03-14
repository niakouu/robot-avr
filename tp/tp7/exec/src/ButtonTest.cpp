#include "ButtonTest.h"

#include <stdio.h>
#include <util/delay.h>

#include "Board.h"
#include "Led.h"

constexpr uint16_t SWITCH_DELAY_MS = 2000;

const char* ButtonTest::NAME = "Button";

const char* ButtonTest::getName() const {
    return ButtonTest::NAME;
}

uint8_t ButtonTest::runTests(void (*log)(const char* format, ...)) const {
    uint8_t fails = 0;
    //TODO
    return fails;
}
