#include "TimerPwmTest.h"

#include <avr/interrupt.h>
#include <string.h>
#include <util/delay.h>

#include "Board.h"
#include "Led.h"
#include "Timer.h"

constexpr uint16_t SWITCH_DELAY_MS = 2000;
constexpr uint8_t WAIT_DELAY_MS = 25;
const char* TimerPwmTest::NAME = "TimerPwmTest";

namespace {
    volatile uint32_t gticks = 0;
} // namespace

const char* TimerPwmTest::getName() const {
    return TimerPwmTest::NAME;
}

uint8_t TimerPwmTest::runTests(void (*log)(const char* format, ...)) const {
    constexpr const float RATIOS[]{0.0F, 0.25F, 0.5F, 0.75F, 1.0F};

    Timer2& timer2 = Board::get().getTimer2();
    Timer2::ConfigPwm config{
        .prescaler = TimerPrescalerAsynchronous::Value::CLK_DIV_8,
        .compareOutputModeA = TimerCompareOutputModeA::CLEAR,
        .compareOutputModeB = TimerCompareOutputModeB::CLEAR};

    Button& button = Board::get().getButton();
    log("Press button to start spining the left motor.");
    while (!button.isEvent()) {
        _delay_ms(WAIT_DELAY_MS);
    }
    button.consumeEvent();

    for (const float ratio : RATIOS) {
        config.speedA = static_cast<uint8_t>(ratio * UINT8_MAX);
        config.speedB = static_cast<uint8_t>(0);

        timer2.setAsPwm(config);
        timer2.start();
        log("Starting timer for left motor at speed: %d\n", config.speedA);

        _delay_ms(SWITCH_DELAY_MS);
    }

    log("Press button to start spining the right motor.");
    while (!button.isEvent()) {
        _delay_ms(WAIT_DELAY_MS);
    }
    button.consumeEvent();

    for (const float ratio : RATIOS) {
        config.speedA = static_cast<uint8_t>(0);
        config.speedB = static_cast<uint8_t>(ratio * UINT8_MAX);

        timer2.setAsPwm(config);
        timer2.start();
        log("Starting timer for right motor at speed: %d\n", config.speedB);

        _delay_ms(SWITCH_DELAY_MS);
    }

    log("Press button to start spining both motors.");
    while (!button.isEvent()) {
        _delay_ms(WAIT_DELAY_MS);
    }
    button.consumeEvent();

    for (const float ratio : RATIOS) {
        config.speedA = static_cast<uint8_t>(ratio * UINT8_MAX);
        config.speedB = static_cast<uint8_t>(ratio * UINT8_MAX);

        timer2.setAsPwm(config);
        timer2.start();
        log("Starting timer for both motors at speed: %d\n", config.speedB);

        _delay_ms(SWITCH_DELAY_MS);
    }

    timer2.stop();
    return 0;
}
