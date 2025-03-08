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

ISR(TIMER1_COMPA_vect) {
    Board::get().getTimer1().setCounterExpired();
}

ISR(WDT_vect) {
    Board::get().getWatchdogTimer().setSleepDone();
}

ISR(RTC_OVF_vect, ISR_NAKED) {
    system_tick();
    reti();
}

static void initialization() {
    sei();

    Board& board = Board::get();
    Uart& uart0 = board.getUart0();
    uart0.configure(BAUD_RATE, false, Uart::Parity::DISABLED,
                    Uart::StopBit::ONE_BIT);

    stdout = uart0.getEmulatedFile();
    stdin = uart0.getEmulatedFile();
    uart0.start();
}

int main() {
    initialization();
    Board& board = Board::get();
    Timer1& timer1 = board.getTimer1();
    const BidirectionalLed bidirectionalLed{Pin::Region::A, Pin::Id::P1,
                                            Pin::Id::P0};
    WatchdogTimer& watchdogTimer = board.getWatchdogTimer();

    bidirectionalLed.setColor(BidirectionalLed::Color::RED);

    Timer1::ConfigCounter configCounter =
        Timer1::ConfigCounter::fromMilliseconds(
            2000, TimerCompareOutputModeA::TOGGLE);

    timer1.setAsCounter(configCounter);
    printf("DDRA: %u\n", DDRA);
    printf("tcnt1: %u\n", TCNT1);
    printf("OCR1A: %u\n", OCR1A);
    printf("OCR1B: %u\n", OCR1B);
    printf("TCCR1A: %u\n", TCCR1A);
    printf("TCCR1B: %u\n", TCCR1B);
    printf("TCCR1C: %u\n", TCCR1C);
    printf("TINSK: %u\n", TIMSK1);

    timer1.start();
    printf("started: TCCR1B: %u\n", TCCR1B);
    printf("started: SREG: %u\n", SREG);

    do {
        // printf("%d\n", TCNT1);
        watchdogTimer.sleep(15, WatchdogTimer::SleepMode::IDLE);
        printf("t:%d\n", time(nullptr));
    } while (!timer1.isCounterExpired());

    bidirectionalLed.setColor(BidirectionalLed::Color::GREEN);
    timer1.stop();
    while (true) {
    }
    return 0;
}