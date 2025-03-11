#include "Led.h"

#include <avr/io.h>
#include <util/delay.h>
#include "Board.h"
#include "Uart.h"

constexpr uint16_t SWITCH_DELAY_MS = 2000;
constexpr uint8_t AMBER_LOOP_TIME = SWITCH_DELAY_MS / 10;
constexpr uint16_t BAUD_RATE = 2400;

namespace {
    void initialization() {
        Uart& uart0 = Board::get().getUart0();
        uart0.configure(BAUD_RATE, false, Uart::Parity::DISABLED, Uart::StopBit::ONE_BIT);
        stdout = uart0.getEmulatedFile();
        stdin = uart0.getEmulatedFile();
        uart0.start();
    }
}

int main() {
    initialization();
    printf("Led Test Start.\n");
    const BidirectionalLed bidirectionalLed(Pin::Region::A, Pin::Id::P1, Pin::Id::P0);
    bidirectionalLed.setColor(BidirectionalLed::Color::RED);
    printf("\tRed:\n\t\tAP1: %d\n\t\tAP0: %d\n", bit_is_set(PORTA, PORTA1) >> PORTA1, bit_is_set(PORTA, PORTA0) >> PORTA0);
    _delay_ms(SWITCH_DELAY_MS);
    bidirectionalLed.setColor(BidirectionalLed::Color::GREEN);
    printf("\tGreen:\n\t\tAP1: %d\n\t\tAP0: %d\n", bit_is_set(PORTA, PORTA1) >> PORTA1, bit_is_set(PORTA, PORTA0) >> PORTA0);
    _delay_ms(SWITCH_DELAY_MS);
    printf("\tCheck bidirectional led for amber.\n");
    bidirectionalLed.executeAmber(SWITCH_DELAY_MS);
    printf("Led Test Stop.\n");
    return 0;
}