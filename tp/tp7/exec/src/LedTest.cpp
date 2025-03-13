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
    uint8_t fails = 0;

    const BidirectionalLed bidirectionalLed{Pin::Region::A, Pin::Id::P1,
                                            Pin::Id::P0};

    log("Make sure bidirectional is configured this way:\n");
    log("  positive: port A1\n");
    log("  negative: port A0\n");

    // Red
    log("Going red for %dms\n", SWITCH_DELAY_MS);
    bidirectionalLed.setColor(BidirectionalLed::Color::RED);

    _delay_ms(static_cast<double>(SWITCH_DELAY_MS) / 2);
    if (bit_is_set(PORTA, PORTA0) != 0 || bit_is_set(PORTA, PORTA1) == 0)
        ++fails;
    _delay_ms(static_cast<double>(SWITCH_DELAY_MS) / 2);

    // Green
    log("Going green for %dms\n", SWITCH_DELAY_MS);
    bidirectionalLed.setColor(BidirectionalLed::Color::GREEN);

    _delay_ms(static_cast<double>(SWITCH_DELAY_MS) / 2);
    if (bit_is_set(PORTA, PORTA0) == 0 || bit_is_set(PORTA, PORTA1) != 0)
        ++fails;
    _delay_ms(static_cast<double>(SWITCH_DELAY_MS) / 2);

    // Amber
    log("Going amber for %dms. Please verify the color\n",
        SWITCH_DELAY_MS);

    bidirectionalLed.executeAmber(SWITCH_DELAY_MS);

    return fails;
}
