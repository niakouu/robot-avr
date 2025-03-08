
#define F_CPU 8000000
#include <util/delay.h>
#include <avr/io.h>
#include "Board.h"

int main() {
    const Board& board = Board::get();
    const Uart& uart0 = board.getUart0();
    uart0.configure(2600, false, Uart::Parity::ODD, Uart::StopBit::ONE_BIT);
    uart0.start(false);
    stdout = uart0.getEmulatedFile();
    stdin = uart0.getEmulatedFile();

    while (true)
    {
        printf("%c\n", getchar());
    }
    
    return 0;
}