#include <avr/interrupt.h>
#include <stdio.h>

#include "Board.h"
#include "debug.h"

namespace {
    constexpr const uint16_t BAUD_RATE = 2400;
    constexpr const uint8_t BLOCK_SIZE = 64;
} // namespace

int main() {
    Uart& uart = Board::get().getUart0();
    uart.configure(::BAUD_RATE, false, Uart::Parity::DISABLED,
                   Uart::StopBit::ONE_BIT);
    uart.start();
    stdout = uart.getEmulatedFile();
    INFO("Ready to receive\n");

    const Memory24& memory = Board::get().getMemory();

    uint16_t size = uart.receive() << UINT8_WIDTH;
    size |= uart.receive();

    uint8_t block[BLOCK_SIZE];
    block[0] = static_cast<uint8_t>(size >> UINT8_WIDTH);
    block[1] = static_cast<uint8_t>(size);

    for (uint16_t address = sizeof(size); address < size; ++address) {
        if (address % sizeof(block) == 0) {
            memory.write(address - sizeof(block), block, sizeof(block));
        }

        block[address % sizeof(block)] = uart.receive();
    }

    memory.write(size - (size % sizeof(block)), block, size % sizeof(block));

#ifdef DEBUG
    INFO("size is: %d\n", size);
    for (uint16_t i = 0; i < size; ++i) {
        fprintf(Board::get().getUart0().getEmulatedFile(), "%02x ", memory.read(i));
    }
    uart.transmit('\n');
#endif

    INFO("All done!\n");

    return 0;
}