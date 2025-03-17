#include "MovementManagerTest.h"

#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>

#include "Board.h"
#include "Motor.h"
#include "MovementManager.h"
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

    MovementManager<uint16_t, TimerPrescalerSynchronous> movementManager{
        Board::get().getTimer1(),
        {Pin(Pin::Region::D, Pin::Id::P5, Pin::Direction::OUT), 0},
        {Pin(Pin::Region::D, Pin::Id::P4, Pin::Direction::OUT), 0}};

    log("Left should be enable: D7 direction: D5\n");
    log("Left should be enable: D6 direction: D4\n");

    movementManager.moveForward(1.0f);
    _delay_ms(WAIT_DELAY_MS);
    movementManager.moveBackward(1.0f);
    _delay_ms(WAIT_DELAY_MS);
    movementManager.moveLeft(1.0f, 0.5f);
    _delay_ms(WAIT_DELAY_MS);
    movementManager.moveRight(1.0f, 0.5f);
    _delay_ms(WAIT_DELAY_MS);

    return fails;
}