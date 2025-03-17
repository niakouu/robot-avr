#include "ButtonTest.h"

#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "Board.h"
#include "Led.h"

constexpr uint16_t MILLISECONDS_IN_SECONDS = 1000;
constexpr uint16_t WAIT_DELAY_MS = 5000;

const char* ButtonTest::NAME = "Button";

const char* ButtonTest::getName() const {
    return ButtonTest::NAME;
}

uint8_t ButtonTest::runTests(void (*log)(const char* format, ...)) const {
    uint8_t fails = 0;
    Button& button = Board::get().getButton();
    log("Please press the button in the next %d seconds\n", WAIT_DELAY_MS / MILLISECONDS_IN_SECONDS);
    _delay_ms(WAIT_DELAY_MS);

    if (!button.isPressed()) {
        ++fails;
    }

    if (!button.isEvent()) {
        ++fails;
    }

    button.consumeEvent();

    if (button.isEvent()) {
        ++fails;
    }
    
    return fails;
}
