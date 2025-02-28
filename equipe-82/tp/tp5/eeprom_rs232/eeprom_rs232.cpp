#define BAUD 2400
#define F_CPU 8000000UL
#include <avr/wdt.h>
#include <string.h>
#include <util/setbaud.h>

#include "memory_24.h"
#include "sleep.h"

constexpr uint16_t START_ADDR = 0x0;
constexpr uint8_t STR[] = "BANANA PUMPKIN!:)\n\0";

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

    Memory24CXXX mem;
    uint8_t data[sizeof(STR)];

    mem.write(START_ADDR, STR, sizeof(STR));
    rawSleep(WDTO_15MS, SLEEP_MODE_PWR_DOWN);
    mem.read(START_ADDR, data, sizeof(data));

    for (uint8_t i = 0; i < 100; i++) {
        for (uint8_t c : data) {
            transmitUART(c);
        }
    }
}