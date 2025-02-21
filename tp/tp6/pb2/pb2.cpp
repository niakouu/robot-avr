//
// Travail : TP2
// Section # : 4
// Équipe # : 82
// Correcteur : CORRECTEUR
// ----------------------------------------------------------
// Développeurs : Edelina Alieva, Kyle Bouchard
// Description du pb1.cpp :
// Identification matérielles : Boutton poussoir noir connecté au port D2 de
//                              l'Atmega324PA (entré)
//                              DEL bidirectionelle sur la carte mère:
//                                  - DEL côté positive connecté à la broche A2
//                                  (sortie)
//                                  - DEL côté négative connecté à la broche A1
//                                  (sortie)
#define BAUD 2400
#define F_CPU 8000000UL
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <stdio.h>
#include <util/delay.h>
#include <util/setbaud.h>

#include "Led.h"
#include "Pin.h"
#include "sleep.h"
#include "Adc.h"

constexpr uint8_t HIGH_LIGHT_THRESHOLD = 0xE6;
constexpr uint8_t LOW_LIGHT_THRESHOLD = 0xA3;

int transmitUART(char data, FILE* stream);
static FILE mystdout{.flags = 0x0002,
                     .put = transmitUART,
                     .get = __null,
                     .udata = 0};

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
                & ~(_BV(UPM01) | _BV(UPM00) | _BV(USBS0) | _BV(UCSZ02)
                    | _BV(UCPOL0)));
}

int transmitUART(char data, FILE* stream) {
    while ((UCSR0A & _BV(UDRE0)) == 0)
        ;
    UDR0 = data;
    return 0;
}

void initialization(void) {
    cli();
    initializeUART();
    stdout = &mystdout;

    EIMSK |= _BV(INT0); // activate interrupt 0

    // ISR called on every edge
    EICRA &= ~(_BV(ISC01));
    EICRA |= _BV(ISC00);

    sei();
}

int main() {
    initialization();

    Adc adc;
    BidirectionalLed bidirectionalLed{Pin::Region::B, Pin::Id::P1, Pin::Id::P0};

    uint8_t data;
    while (true)
    {
        data = static_cast<uint8_t>(adc.read(0) >> 2);
        printf("%02x\n", data);
        if (data >= HIGH_LIGHT_THRESHOLD) {
            bidirectionalLed.setColor(BidirectionalLed::Color::RED);
            rawSleep(WDTO_15MS, SLEEP_MODE_IDLE);
        }
        else if (data < LOW_LIGHT_THRESHOLD) {
            bidirectionalLed.setColor(BidirectionalLed::Color::GREEN);
            rawSleep(WDTO_15MS, SLEEP_MODE_IDLE);
        }
        else 
            bidirectionalLed.executeAmberCycle();
    }

    return 0;
}