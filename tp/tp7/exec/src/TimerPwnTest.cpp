#include <avr/interrupt.h>
#include <util/delay.h>

#include "Board.h"
#include "Led.h"
#include "Timer.h"
#include "TimerPwmTest.h"

constexpr uint16_t SWITCH_DELAY_MS = 2000;
constexpr uint8_t DELAY_15_MS = 15;
constexpr uint8_t OFFSET_MS = 50;
constexpr uint16_t MS_PER_SEC = 1000;
const char* TimerPwmTest::NAME = "TimerPwmTest";

namespace {
    volatile uint32_t gticks = 0;
} // namespace

ISR(TIMER1_COMPA_vect) {
    Board::get().getTimer1().setCounterExpired();
}

ISR(TIMER2_COMPA_vect) {
    ::gticks++;
}

const char* TimerPwmTest::getName() const {
    return TimerPwmTest::NAME;
}

uint8_t TimerPwmTest::runTests(void (*log)(const char* format, ...)) const {
    uint8_t fails = 0;
    constexpr const float RATIOS[]{0.0F, 0.25F, 0.5F, 0.75F, 1.0F};

    Timer2& timer2 = Board::get().getTimer2();
    Timer2::ConfigPwm config{
        .prescaler = TimerPrescalerAsynchronous::Value::CLK_DIV_8,
        .compareOutputModeA = TimerCompareOutputModeA::CLEAR,
        .compareOutputModeB = TimerCompareOutputModeB::CLEAR};

    for (const float ratio : RATIOS) {
        config.speedA = static_cast<uint8_t>(ratio * UINT8_MAX);
        config.speedB = static_cast<uint8_t>(ratio * UINT8_MAX);

        timer2.setAsPwm(config);

        log("Starting timer at speed: %d\n", config.speedA);

        _delay_ms(SWITCH_DELAY_MS);
        // TODO
    }

    return fails;
}
