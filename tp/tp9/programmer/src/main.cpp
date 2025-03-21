#include <avr/interrupt.h>
#include <stdio.h>

#include "Board.h"
#include "debug.h"

namespace {
    constexpr const uint16_t BAUD_RATE = 2400;
} // namespace

int main() {
    const Uart& uart = Board::get().getUart0();
    uart.configure(::BAUD_RATE, false, Uart::Parity::DISABLED,
                   Uart::StopBit::ONE_BIT);
    uart.start();
    INFO("Ready to receive\n");

    const Memory24& memory = Board::get().getMemory();

    uint16_t size = uart.receive() << UINT8_WIDTH;
    size |= uart.receive();

    // We must go through reinterpret_cast because of pointer cast.
    // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
    memory.write(0, reinterpret_cast<const uint8_t *>(&size), sizeof(size));
    // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)

    uint16_t errors = 0;
    for (uint16_t address = sizeof(size); address < size; ++address) {
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