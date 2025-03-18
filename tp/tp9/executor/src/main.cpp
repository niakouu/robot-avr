#include <avr/interrupt.h>
#include <stdio.h>
#include "Board.h"
#include "Emulator.h"

constexpr uint16_t BAUD_RATE = 2400;

ISR(INT0_vect) {
    Board::get().getButton().setPressed();
    EIFR |= _BV(INTF0); // activate interrupt 0
}

ISR(WDT_vect) {
    Board::get().getWatchdogTimer().setSleepDone();
}

void initializer() {
    cli();
    Uart& uart0 = Board::get().getUart0();
    stdout = uart0.getEmulatedFile();
    uart0.configure(::BAUD_RATE, false, Uart::Parity::DISABLED,
                    Uart::StopBit::ONE_BIT);
    uart0.start();
    sei();
}

int main() {
    initializer();
    Emulator emulator;
    Memory24 memory = Board::get().getMemory();
    uint16_t data = 0;
    // We must go through reinterpret_cast because of pointer cast.
    // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
    memory.read(emulator.getInstructionPointer(), reinterpret_cast<uint8_t*>(&data), sizeof(data));
    // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)

    while(!emulator.isDone()) {
        emulator.executeNextInstruction(data);
    }

    return 0;
}