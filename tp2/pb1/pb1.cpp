// 
// Travail : TP2
// Section # : 4
// Équipe # : 82
// Correcteur : CORRECTEUR
// ----------------------------------------------------------
// Développeurs : Edelina Alieva, Kyle Bouchard
// Description du pb1.cpp : Après avoir appuyer 3 fois sur le bouton poussoir 
//                          de la carte mère, la DEL tourne verte pendant 2 secondes.
//                          Ensuite, la del retourne à son état initial.
// Identification matérielles : Button poussoir connecté à port D2 de l'Atmega324PA
//                              DEL côté positive connecté à broche A2
//                              DEL côté négative connecté à broche A1
// Table des états :
// +---------------+---------+---------------+-----+
// | Current State | pressed | Next State    | led |
// +---------------+---------+---------------+-----+
// | INIT          | 0       | INIT          | 0   |
// | INIT          | 1       | PRESSED_ONCE  | 0   |
// | PRESSED_ONCE  | 0       | PRESSED_ONCE  | 0   |
// | PRESSED_ONCE  | 1       | PRESSED_TWICE | 0   |
// | PRESSED_TWICE | 0       | PRESSED_TWICE | 0   |
// | PRESSED_TWICE | 1       | LIGHT_ON      | 0   |
// | LIGHT_ON      | X       | INIT          | 1   |
// +---------------+---------+---------------+-----+

#include <avr/io.h>
#include <avr/wdt.h>
#include "sleep.h"

enum class State { INIT, PRESSED_ONCE, PRESSED_TWICE, LIGHT_ON };
constexpr uint8_t BUTTON_BIT_MASK = _BV(PIND2);
constexpr uint8_t LED_GREEN = _BV(PORTA0) & ~(_BV(PORTA1));
constexpr uint8_t LED_OFF = 0;

void waitForButtonPress() {
    while (!(PIND & BUTTON_BIT_MASK)) {
        sleep(WDTO_15MS, SLEEP_MODE_PWR_DOWN);
    }

    while (PIND & BUTTON_BIT_MASK) {
        sleep(WDTO_15MS, SLEEP_MODE_PWR_DOWN);
    }
}

int main() {
    DDRA |= _BV(DDA1) | _BV(DDA0);
    DDRD &= ~(_BV(DDD2));

    State curState = State::INIT;
    while (true)
    {
        switch (curState)
        {
        case State::INIT:
            PORTA = LED_OFF;
            waitForButtonPress();
            curState = State::PRESSED_ONCE;
            break;
        case State::PRESSED_ONCE:
            waitForButtonPress();
            curState = State::PRESSED_TWICE;
            break;
        case State::PRESSED_TWICE:
            waitForButtonPress();
            curState = State::LIGHT_ON;
            break;
        case State::LIGHT_ON:
            PORTA = LED_GREEN;
            sleep(WDTO_2S, SLEEP_MODE_PWR_DOWN);
            curState = State::INIT;
            break;
        default:;
        }
    }
}