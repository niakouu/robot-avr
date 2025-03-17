#include "LedTest.h"

#include <stdio.h>
#include <util/delay.h>

#include "Board.h"
#include "Led.h"

constexpr uint16_t SWITCH_DELAY_MS = 2000;

const char* LedTest::NAME = "Led";

const char* LedTest::getName() const {
    return LedTest::NAME;
}

uint8_t LedTest::runTests(void (*log)(const char* format, ...)) const {
    const BidirectionalLed bidirectionalLed{Pin::Region::A, Pin::Id::P1,
                                            Pin::Id::P0};

    log("Positive A1, Negative A0, verify manually\n");

    // Red
    log("RED\n", SWITCH_DELAY_MS);
    bidirectionalLed.setColor(BidirectionalLed::Color::RED);

    _delay_ms(static_cast<double>(SWITCH_DELAY_MS));

    // Green
    log("GREEN\n", SWITCH_DELAY_MS);
    bidirectionalLed.setColor(BidirectionalLed::Color::GREEN);

    _delay_ms(static_cast<double>(SWITCH_DELAY_MS));

    // Green
    log("OFF\n", SWITCH_DELAY_MS);
    bidirectionalLed.setColor(BidirectionalLed::Color::OFF);

    _delay_ms(static_cast<double>(SWITCH_DELAY_MS));

    // Amber
    log("AMBER\n");

    bidirectionalLed.executeAmber(SWITCH_DELAY_MS);

    return 0;
}
