#include <avr/interrupt.h>
#include <stdio.h>

ISR(INT0_vect) {
    // Board::get().getButton().setPressed();
    EIFR |= _BV(INTF0); // activate interrupt 0
}

ISR(WDT_vect) {
    // Board::get().getWatchdogTimer().setSleepDone();
}

int main() {
    sei();

    return 0;
}