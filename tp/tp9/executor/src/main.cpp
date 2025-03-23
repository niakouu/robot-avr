#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>

#include "Board.h"
#include "Emulator.h"
#include "Led.h"
#include "debug.h"
#include "WatchdogTimer.h"

constexpr uint16_t BAUD_RATE = 2400;

ISR(INT0_vect) {
    Board::get().getButton().setPressed();
    EIFR |= _BV(INTF0); // activate interrupt 0
}

ISR(WDT_vect) {
    Board::get().getWatchdogTimer().setSleepDone();
}

namespace {
    void initializer() {
        cli();
        Uart& uart0 = Board::get().getUart0();
        stdout = uart0.getEmulatedFile();
        uart0.configure(::BAUD_RATE, false, Uart::Parity::DISABLED,
                        Uart::StopBit::ONE_BIT);
        uart0.start();
        sei();
    }
} // namespace

int main() {
    initializer();
    Emulator emulator;
    uint16_t data = 0;

    const Memory24& memory = Board::get().getMemory();
    const uint16_t size = (memory.read(0) << UINT8_WIDTH) | memory.read(1);

    emulator.bootStart();

    while (!emulator.isDone()) {
        const uint16_t address =
            sizeof(size) + emulator.getInstructionPointer();

        if (address >= size) {
            ERROR("End w/o finish\n");
            break;
        }

        data = (memory.read(address) << UINT8_WIDTH) | memory.read(address + 1);

        emulator.executeNextInstruction(data);
    }

    return 0;
}