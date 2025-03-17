#include "MovementManagerTest.h"

#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>

#include "Board.h"
#include "Motor.h"
#include "Pin.h"

constexpr uint16_t MILLISECONDS_IN_SECONDS = 1000;
constexpr uint16_t WAIT_DELAY_MS = 5000;
constexpr uint16_t SWITCH_DELAY_MS = 2000;

const char* MovementManagerTest::NAME = "MovementManager";

const char* MovementManagerTest::getName() const {
    return MovementManagerTest::NAME;
}

uint8_t MovementManagerTest::runTests(void (*log)(const char* format,
                                                  ...)) const {
    uint8_t fails = 0;

    const Motor<uint8_t> motor(Pin(Pin::Region::A, Pin::Id::P1, Pin::Direction::OUT),
                          0);

    log("Left should be enable: D7 direction: D5\n");
    log("Left should be enable: D6 direction: D4\n");

    _delay_ms(WAIT_DELAY_MS);

    log("Going forward for %dms\n", SWITCH_DELAY_MS);
    motor.move(1, true);

    _delay_ms(static_cast<double>(SWITCH_DELAY_MS) / 2);
    if (bit_is_set(PORTA, PORTA1) == 0)
        ++fails;
    _delay_ms(static_cast<double>(SWITCH_DELAY_MS) / 2);

    log("Going backwards for %dms\n", SWITCH_DELAY_MS);
    motor.move(1, false);

    _delay_ms(static_cast<double>(SWITCH_DELAY_MS) / 2);
    if (bit_is_set(PORTA, PORTA1) != 0)
        ++fails;
    _delay_ms(static_cast<double>(SWITCH_DELAY_MS) / 2);

    log("Motor stops", SWITCH_DELAY_MS);
    motor.move(0, true);

    return fails;
}