#include <avr/interrupt.h>
#include <stdio.h>

#include "Board.h"
#include "debug.h"

constexpr const uint16_t BAUD_RATE = 2400;

int main() {
    Uart& uart = Board::get().getUart0();
    uart.configure(::BAUD_RATE, false, Uart::Parity::DISABLED,
                   Uart::StopBit::ONE_BIT);
    uart.start();
    stdout = uart.getEmulatedFile();

    INFO("Hello, world!\n");

    return 0;
}