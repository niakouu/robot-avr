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

namespace {
    volatile uint32_t gticks = 0;

    void initialization() {
        sei();

        Board& board = Board::get();
        Uart& uart0 = board.getUart0();
        uart0.configure(BAUD_RATE, false, Uart::Parity::DISABLED,
                        Uart::StopBit::ONE_BIT);

        stdout = uart0.getEmulatedFile();
        stdin = uart0.getEmulatedFile();
        uart0.start();
    }
} // namespace

ISR(TIMER1_COMPA_vect) {
    Board::get().getTimer1().setCounterExpired();
}

ISR(WDT_vect) {
    Board::get().getWatchdogTimer().setSleepDone();
}

ISR(TIMER2_COMPA_vect) {
    ::gticks++;
}

int main() {
    initialization();
    Board& board = Board::get();
    Timer1& timer1 = board.getTimer1();
    Timer2& timer2 = board.getTimer2();
    const BidirectionalLed bidirectionalLed{Pin::Region::A, Pin::Id::P1,
                                            Pin::Id::P0};
    WatchdogTimer& watchdogTimer = board.getWatchdogTimer();

    bidirectionalLed.setColor(BidirectionalLed::Color::RED);

    timer1.setAsCounter(Timer1::ConfigCounter::fromMilliseconds(
        RED_LED_TIME_MS, TimerCompareOutputModeA::TOGGLE));

    timer2.setAsCounter(
        {.maxTicks = 0,
         .prescaler = TimerPrescalerAsynchronous::Value::CLK_DIV_1024});
    timer2.start();

    printf("DDRA: %u\n", DDRA);
    printf("tcnt1: %u\n", TCNT1);
    printf("OCR1A: %u\n", OCR1A);
    printf("OCR1B: %u\n", OCR1B);
    printf("TCCR1A: %u\n", TCCR1A);
    printf("TCCR1B: %u\n", TCCR1B);
    printf("TCCR1C: %u\n", TCCR1C);
    printf("TINSK: %u\n", TIMSK1);

    timer1.start();
    const uint32_t startTime = ::gticks;
    printf("started: TCCR1B: %u\n", TCCR1B);
    printf("started: SREG: %u\n", SREG);

    while (!timer1.isCounterExpired())
    {
        watchdogTimer.sleep(DELAY_15_MS, WatchdogTimer::SleepMode::IDLE);
    }

    printf("Took start: %lu end: %lu ticks\n", startTime, ::gticks);

    bidirectionalLed.setColor(BidirectionalLed::Color::GREEN);
    timer1.stop();
    while (true) {
    }
    return 0;
}