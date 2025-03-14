#include "PhotoresistanceTest.h"

#include <stdio.h>
#include <util/delay.h>

#include "Board.h"
#include "Led.h"

constexpr uint16_t SWITCH_DELAY_MS = 2000;

const char* PhotoresistanceTest::NAME = "Photoresistance";

const char* PhotoresistanceTest::getName() const {
    return PhotoresistanceTest::NAME;
}

uint8_t PhotoresistanceTest::runTests(void (*log)(const char* format, ...)) const {
    uint8_t fails = 0;
    //TODO
    return fails;
}
