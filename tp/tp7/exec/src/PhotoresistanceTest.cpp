#include "PhotoresistanceTest.h"

#include <stdio.h>
#include <util/delay.h>

#include "Board.h"
#include "Led.h"
#include "Photoresistance.h"

constexpr uint16_t WAIT_DELAY_MS = 250;

const char* PhotoresistanceTest::NAME = "Photoresistance";

const char* PhotoresistanceTest::getName() const {
    return PhotoresistanceTest::NAME;
}

uint8_t PhotoresistanceTest::runTests(void (*log)(const char* format,
                                                  ...)) const {
    constexpr Pin::Id photoresistancePinId = Pin::Id::P5;
    uint8_t fails = 0;
    Button& button = Board::get().getButton();
    const Photoresistance photoresistance{Board::get().getAdc(),
                                          photoresistancePinId};

    log("Please plug the photoresistance in pin A%d\n",
        static_cast<uint8_t>(photoresistancePinId));

    // High
    log("Please press the button once the light is very close to the "
        "sensor.\n");

    while (!(button.isEvent() && button.isPressed())) {
        _delay_ms(WAIT_DELAY_MS);
    }

    button.consumeEvent();

    if (photoresistance.readIntensity() != Photoresistance::Intensity::HIGH) {
        ++fails;
    }

    // Medium
    log("Please press the button once the light is ambient to the sensor\n");

    while (!(button.isEvent() && button.isPressed())) {
        _delay_ms(WAIT_DELAY_MS);
    }

    button.consumeEvent();

    if (photoresistance.readIntensity() != Photoresistance::Intensity::MEDIUM) {
        ++fails;
    }

    // Low
    log("Please press the button once you remove light from the sensor\n");

    while (!button.isEvent()) {
        _delay_ms(WAIT_DELAY_MS);
    }

    button.consumeEvent();

    if (photoresistance.readIntensity() != Photoresistance::Intensity::LOW) {
        ++fails;
    }

    return fails;
}
