#include <stdint.h>
#include <stdio.h>
#include "Board.h"
#include "Uart.h"

constexpr uint16_t BAUD_RATE = 2400;

int main() {
    const Board& board = Board::get();
    const Uart& uart0 = board.getUart0();
    uart0.configure(BAUD_RATE, false, Uart::Parity::DISABLED,
                    Uart::StopBit::ONE_BIT);
    uart0.start(false);
    stdout = uart0.getEmulatedFile();
    stdin = uart0.getEmulatedFile();

    while (true) {
        putchar(getchar());
    }

    return 0;
}