#include <avr/interrupt.h>
#include <stdio.h>

#include "Board.h"
#include "Robot.h"
#include "debug.h"
#include "DistanceSensor.h"

constexpr const uint16_t BAUD_RATE = 2400;

static volatile Button EXTRA_BUTTON { Button::Interrupt::I1, false };

ISR(WDT_vect) {
    Board::get().getWatchdogTimer().setSleepDone();
}

ISR(INT0_vect) {
    Board::get().getButton().setPressed();
}

ISR(INT1_vect) {
    EXTRA_BUTTON.setPressed();
}

int main() {
    Uart& uart = Board::get().getUart0();
    uart.configure(::BAUD_RATE, false, Uart::Parity::DISABLED,
                   Uart::StopBit::ONE_BIT);
    uart.start();
    stdout = uart.getEmulatedFile();

    Button extraButton { Button::Interrupt::I1, false };
    sei();

    const DistanceSensor ds{Pin::Id::P5};

    while (true) {
        Robot::get().followLine();

        Board::get().getWatchdogTimer().sleep(100, WatchdogTimer::SleepMode::IDLE);
    };

    return 0;
}