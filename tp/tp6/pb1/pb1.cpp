//
// Travail : TP2
// Section # : 4
// Équipe # : 82
// Correcteur : CORRECTEUR
// ----------------------------------------------------------
// Développeurs : Edelina Alieva, Kyle Bouchard
// Description du pb1.cpp : Programme qui configure le nombre de fois que la DEL
//                          bicolore devrait clignoter, en prenant en compte
//                          combien de temps le bouton poussoir externe a été
//                          appuyé.
// Identification matérielles : Boutton poussoir blanc connecté au port D2 de
//                              l'Atmega324PA (entré)
//                              DEL bidirectionelle sur la carte mère:
//                                  - DEL côté positive connecté à la broche B2
//                                  (sortie)
//                                  - DEL côté négative connecté à la broche B1
//                                  (sortie)
#define F_CPU 8'000'000UL
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include "Led.h"
#include "Pin.h"
#include "sleep.h"

constexpr uint16_t PRESCALER = 1024;
constexpr uint16_t HALF_SECOND_MS = 500;
constexpr uint8_t TICKS_PER_SECOND = 10;
constexpr uint8_t FLASH_TIME_MS = 10;
constexpr uint8_t MAX_TICKS = 120;
constexpr uint8_t INTERRUPT_DELAY_TIME_MS = 15;

const Pin gButton{Pin::Region::D, Pin::Direction::IN, Pin::Id::P2};

volatile bool gButtonDown = false;
volatile uint8_t gTicks = 0;

ISR(INT0_vect) {
    _delay_ms(INTERRUPT_DELAY_TIME_MS); // anti-rebound
    ::gButtonDown = !::gButton.read();

    EIFR |= _BV(INTF0); // activate interrupt 0
}

ISR(TIMER1_COMPA_vect) {
    ++::gTicks;
}

void startTimer(uint16_t ticks) {
    // mode CTC timer 1 with clock 1024
    TCNT1 = 0;
    OCR1A = ticks;
    TCCR1A = _BV(COM1A1) | (TCCR1A & ~_BV(COM1A0));
    TCCR1B = _BV(CS12) | _BV(CS10) | _BV(WGM12) | (TCCR1B & ~_BV(CS11));
    TCCR1C = 0;
    TIMSK1 |= _BV(OCIE1A);
}

void stopTimer() {
    TCCR1B &= ~(_BV(CS12) | _BV(CS11) | _BV(CS10));
}

void initialization(void) {
    cli();

    EIMSK |= _BV(INT0); // activate interrupt 0

    // ISR called on every edge
    EICRA &= ~(_BV(ISC01));
    EICRA |= _BV(ISC00);

    sei();
}

int main() {
    initialization();
    BidirectionalLed bidirectionalLed{Pin::Region::B, Pin::Id::P1, Pin::Id::P0};

    bidirectionalLed.setColor(BidirectionalLed::Color::OFF);

    // Wait for button to be pressed
    while (!gButtonDown) {
        rawSleep(WDTO_30MS, SLEEP_MODE_IDLE);
    }

    // Start timer and wait until either timer expires or button is up
    startTimer(F_CPU / PRESCALER / TICKS_PER_SECOND);
    while (::gTicks < MAX_TICKS && ::gButtonDown) {
        rawSleep(WDTO_30MS, SLEEP_MODE_IDLE);
    }
    stopTimer();

    // Flash LED green for half a second
    for (uint16_t i = 0; i < HALF_SECOND_MS / FLASH_TIME_MS / 2; i++) {
        bidirectionalLed.setColor(BidirectionalLed::Color::GREEN);
        sleep(FLASH_TIME_MS, SLEEP_MODE_IDLE);
        bidirectionalLed.setColor(BidirectionalLed::Color::OFF);
        sleep(FLASH_TIME_MS, SLEEP_MODE_IDLE);
    }

    bidirectionalLed.setColor(BidirectionalLed::Color::OFF);
    rawSleep(WDTO_2S, SLEEP_MODE_IDLE);

    // Flash LED red for the number of ticks.
    for (uint8_t i = 0; i < ::gTicks / 2; i++) {
        bidirectionalLed.setColor(BidirectionalLed::Color::RED);
        rawSleep(WDTO_250MS, SLEEP_MODE_IDLE);
        bidirectionalLed.setColor(BidirectionalLed::Color::OFF);
        rawSleep(WDTO_250MS, SLEEP_MODE_IDLE);
    }

    // Put the LED green for a second, close it and exit.
    bidirectionalLed.setColor(BidirectionalLed::Color::GREEN);
    rawSleep(WDTO_1S, SLEEP_MODE_IDLE);
    bidirectionalLed.setColor(BidirectionalLed::Color::OFF);

    return 0;
}