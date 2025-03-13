#include "TimerPwmTest.h"

#include <avr/interrupt.h>
#include <util/delay.h>

#include "Board.h"
#include "Led.h"
#include "Timer.h"

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

uint8_t TimerPwmTest::runTests(void (*log)(const char* name,
                                               const char* format, ...)) const {
    uint8_t fails = 0;

    Timer2& timer2 = Board::get().getTimer2();
    

    return fails;
}
