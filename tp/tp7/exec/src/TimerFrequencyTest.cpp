#include "TimerFrequencyTest.h"

#include <avr/interrupt.h>
#include <util/delay.h>

#include "Board.h"
#include "Timer.h"

constexpr uint16_t SWITCH_DELAY_MS = 2000;
constexpr uint8_t DELAY_15_MS = 15;
constexpr uint8_t OFFSET_MS = 50;
constexpr uint16_t MS_PER_SEC = 1000;
constexpr const uint32_t FREQUENCIES[]{0, 200, 400, 600, 800, 100};
const char* TimerFrequencyTest::NAME = "TimerFrequencyTest";

namespace {
    volatile uint32_t gticks = 0;
} // namespace

const char* TimerFrequencyTest::getName() const {
    return TimerFrequencyTest::NAME;
}

uint8_t TimerFrequencyTest::runTests(void (*log)(const char* format,
                                                 ...)) const {
    Timer1& timer1 = Board::get().getTimer1();

    for (const uint32_t frequency : FREQUENCIES) {
        // log("Starting timer for buzzer at frequency: %d\n", frequency);
        timer1.setAsPwmFrequency(Timer1::ConfigFrequency::fromFrequency(
            frequency, TimerCompareOutputModeA::CLEAR,
            TimerCompareOutputModeB::CLEAR));
        _delay_ms(SWITCH_DELAY_MS);
    }

    return 0;
}
