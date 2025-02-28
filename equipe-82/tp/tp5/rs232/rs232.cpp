#define BAUD 2400
#define F_CPU 8000000UL
#include <avr/wdt.h>
#include <string.h>
#include <util/setbaud.h>

void initializeUART(void) {
    // 2400 bauds
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

    // allow reception and transmission
    UCSR0A = _BV(UDRE0);
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);

    // 8 bits, 1 stop bits, parity none
    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00)
             | (UCSR0C
                & ~(_BV(UPM01) | _BV(UPM00) | _BV(USBS0)
                    | _BV(UCSZ02) | _BV(UCPOL0)));
}

void transmitUART(uint8_t data) {
    while ((UCSR0A & _BV(UDRE0)) == 0)
        ;
    UDR0 = data;
}

int main() {
    initializeUART();
    const char words[] = "Banana pumpkin!\n";
    for (uint8_t i = 0; i < 100; i++) {
        for (uint8_t c : words) {
            transmitUART(c);
        }
    }
}