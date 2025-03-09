#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <util/delay.h>

#include "Board.h"
#include "Led.h"
#include "Uart.h"
#include "WatchdogTimer.h"
#include "Timer.h"

constexpr uint16_t BAUD_RATE = 2400;
constexpr uint16_t RED_LED_TIME_MS = 2000;
constexpr uint8_t DELAY_15_MS = 15;

ISR(TIMER2_COMPA_vect) {
    Board::get().getTimer2().setCounterExpired();
}

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
    const BidirectionalLed bidirectionalLed{Pin::Region::A, Pin::Id::P1,
                                            Pin::Id::P0};
    WatchdogTimer& watchdogTimer = board.getWatchdogTimer();

    bidirectionalLed.setColor(BidirectionalLed::Color::RED);

    timer2.setAsCounter(Timer2::ConfigCounter::fromMilliseconds(
        RED_LED_TIME_MS, TimerCompareOutputModeA::TOGGLE));

    printf("DDRA: %u\n", DDRA);
    printf("TCNT1: %u\n", TCNT2);
    printf("OCR1A: %u\n", OCR2A);
    printf("OCR1B: %u\n", OCR2B);
    printf("TCCR1A: %u\n", TCCR2A);
    printf("TCCR1B: %u\n", TCCR2B);
    // printf("TCCR1C: %u\n", TCCR2C);
    printf("TINSK: %u\n", TIMSK2);

    timer2.start();
    printf("started: TCCR1B: %u\n", TCCR2B);
    printf("started: SREG: %u\n", SREG);

    while (!timer2.isCounterExpired())
    {
        watchdogTimer.sleep(DELAY_15_MS, WatchdogTimer::SleepMode::IDLE);
    }

    bidirectionalLed.setColor(BidirectionalLed::Color::GREEN);
    timer2.stop();
    while (true) {
    }
    return 0;
}