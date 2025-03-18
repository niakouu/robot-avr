#include <avr/interrupt.h>
#include <stdio.h>

#include "Board.h"
#include "debug.h"

namespace {
    constexpr const uint16_t BAUD_RATE = 2400;
} // namespace

ISR(INT0_vect) {
    Board::get().getButton().setPressed();
    EIFR |= _BV(INTF0); // activate interrupt 0
}

ISR(WDT_vect) {
    Board::get().getWatchdogTimer().setSleepDone();
}

int main() {
    sei();

    const Uart& uart = Board::get().getUart0();
    uart.configure(::BAUD_RATE, false, Uart::Parity::DISABLED,
                   Uart::StopBit::ONE_BIT);
    uart.start();
    INFO("Ready to receive\n");

    Memory24& memory = Board::get().getMemory();

    uint16_t size = uart.receive() << UINT8_WIDTH;
    size |= uart.receive();
    size -= 2; // Size's size is included

    uint16_t errors = 0;
    for (uint16_t address = 0; address < size; ++address) {
        const uint8_t byteToWrite = uart.receive();
        memory.write(address, &byteToWrite, sizeof(uint8_t));

        const uint8_t readBack = memory.read(address);
        if (readBack != byteToWrite)
            ++errors;
    }

    if (errors > 0) {
        ERROR("Got %d errors!\n", errors);
    }

    INFO("All done\n");

    return 0;
}