#include "TimerCounterTest.h"

#include <avr/interrupt.h>
#include <util/delay.h>

#include "Board.h"
#include "Led.h"
#include "Timer.h"

constexpr uint16_t SWITCH_DELAY_MS = 2000;
constexpr uint8_t DELAY_15_MS = 15;
constexpr uint8_t OFFSET_MS = 50;
constexpr uint16_t MS_PER_SEC = 1000;
const char* TimerCounterTest::NAME = "TimerCounterTest";

namespace {
    volatile uint32_t gticks = 0;
} // namespace

ISR(TIMER1_COMPA_vect) {
    Board::get().getTimer1().setCounterExpired();
}

ISR(TIMER2_COMPA_vect) {
    ::gticks++;
}

const char* TimerCounterTest::getName() const {
    return TimerCounterTest::NAME;
}

uint8_t TimerCounterTest::runTests(void (*log)(const char* format, ...)) const {
    uint8_t fails = 0;

    Timer1& timer1 = Board::get().getTimer1();
    Timer2& timer2 = Board::get().getTimer2();

    // Counter Settings
    log("Setting timer 1 counter settings from milliseconds.\n");
    timer1.setAsCounter(Timer1::ConfigCounter::fromMilliseconds(
        SWITCH_DELAY_MS, TimerCompareOutputModeA::TOGGLE));
    log("Setting timer 2 counter settings.\n");
    timer2.setAsCounter(
        {.maxTicks = 0,
         .prescaler = TimerPrescalerAsynchronous::Value::CLK_DIV_1024});

    // Timer start
    log("Starting timer2.\n");
    timer2.start();
    log("Starting timer1.\n");
    timer1.start();

    const uint32_t startTime = ::gticks;

    while (!timer1.isCounterExpired())
        _delay_ms(DELAY_15_MS);

    const uint32_t endTime = ::gticks;
    const uint32_t differenceMs = (endTime - startTime) / (F_CPU / MS_PER_SEC);

    log("Stopping Timer 1.");
    timer1.stop();

    log("Took start: %lu end: %lu ticks\n", startTime, endTime);

    if (differenceMs < SWITCH_DELAY_MS - OFFSET_MS
        || differenceMs > SWITCH_DELAY_MS + OFFSET_MS)
        ++fails;

    log("Stopping Timer 2.");
    timer2.stop();

    return fails;
}
