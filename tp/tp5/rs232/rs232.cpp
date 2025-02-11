#define BAUD 2400
#define F_CPU 8000000UL
#include <avr/wdt.h>
#include <string.h>
#include <util/setbaud.h>

#include "Led.h"
#include "Pin.h"
#include "memory_24.h"
#include "sleep.h"

void initializeUART(void) {
    // 2400 bauds. Nous vous donnons la valeur des deux
    // premiers registres pour vous éviter des complications.
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

    // permettre la réception et la transmission par le UART0
    UCSR0A = _BV(UDRE0);
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);

    // Format des trames: 8 bits, 1 stop bits, sans parité
    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00)
             | (UCSR0C
                & ~(_BV(UMSEL01) | _BV(UMSEL00) | _BV(UPM01) | _BV(UPM00) | _BV(USBS0)
                    | _BV(UCSZ02) | _BV(UCPOL0)));
}

// Du USART vers le PC
void transmitUART(uint8_t data) {
    while ((UCSR0A & _BV(UDRE0)) == 0)
        ;
    UDR0 = data;
}

int main() {
    while (true) {
        char words[] = "Banana pumpkin!\n";
        for (uint8_t i = 0; i < 100; i++) {
            for (uint8_t j = 0; j < sizeof(words); j++) {
                transmitUART(words[j]);
            }
        }
    }
}