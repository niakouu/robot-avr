#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <util/delay.h>

#include "Board.h"
#include "Led.h"
#include "Timer.h"
#include "Uart.h"
#include "WatchdogTimer.h"

constexpr uint16_t BAUD_RATE = 2400;
constexpr uint16_t RED_LED_TIME_MS = 2000;
constexpr uint8_t DELAY_15_MS = 15;
constexpr uint8_t FRACTION_TO_PERCENT = 100;
constexpr float RATIOS[] = {0.0F, 0.25F, 0.5F, 0.75F, 1.0F};

ISR(WDT_vect) {
    Board::get().getWatchdogTimer().setSleepDone();
}

namespace {
    void initialization() {
        sei();

        Uart& uart0 = Board::get().getUart0();
        uart0.configure(BAUD_RATE, false, Uart::Parity::DISABLED,
                        Uart::StopBit::ONE_BIT);

        stdout = uart0.getEmulatedFile();
        stdin = uart0.getEmulatedFile();
        uart0.start();
    }
} // namespace

int main() {
    initialization();
    Board& board = Board::get();
    Timer2& timer2 = board.getTimer2();
    WatchdogTimer& watchdogTimer = board.getWatchdogTimer();

    timer2.setAsCounter(Timer2::ConfigCounter::fromMilliseconds(
        RED_LED_TIME_MS, TimerCompareOutputModeA::TOGGLE));

    while (true) {
        for (const float ratio : RATIOS) {
            timer2.stop();
            timer2.setAsPwm(
                {.prescaler = TimerPrescalerAsynchronous::Value::CLK_DIV_8,
                 .speedA = ratio,
                 .speedB = ratio,
                 .compareOutputModeA = TimerCompareOutputModeA::CLEAR,
                 .compareOutputModeB = TimerCompareOutputModeB::CLEAR});
            timer2.start();
            printf("Ratio: %d\n", static_cast<int>(ratio * FRACTION_TO_PERCENT));
            watchdogTimer.sleep(RED_LED_TIME_MS, WatchdogTimer::SleepMode::IDLE);
        }
    }
    return 0;
}