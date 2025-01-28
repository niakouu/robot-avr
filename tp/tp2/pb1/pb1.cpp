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
// Identification matérielles : Boutton poussoir connecté au port D2 de l'Atmega324PA (entré)
//                              DEL bidirectionelle sur la carte mère:
//                                  DEL côté positive connecté à la broche A2 (sortie)
//                                  DEL côté négative connecté à la broche A1 (sortie)
// Table des états :
// +----------------+---------+----------------+-----+
// | Current State  | pressed | Next State     | led |
// +----------------+---------+----------------+-----+
// | INIT           | 0       | INIT           | 0   |
// | INIT           | 1       | PRESSED_ONCE   | 0   |
// | PRESSED_ONCE   | 0       | RELEASED_ONCE  | 0   |
// | PRESSED_ONCE   | 1       | PRESSED_ONCE   | 0   |
// | RELEASED_ONCE  | 0       | RELEASED_ONCE  | 0   |
// | RELEASED_ONCE  | 1       | PRESSED_TWICE  | 0   |
// | PRESSED_TWICE  | 0       | RELEASED_TWICE | 0   |
// | PRESSED_TWICE  | 1       | PRESSED_TWICE  | 0   |
// | RELEASED_TWICE | 0       | RELEASED_TWICE | 0   |
// | RELEASED_TWICE | 1       | PRESSED_THRICE | 0   |
// | PRESSED_THRICE | 0       | LIGHT_ON       | 0   |
// | PRESSED_THRICE | 1       | PRESSED_THRICE | 0   |
// | LIGHT_ON       | X       | INIT           | 1   |
// +----------------+---------+----------------+-----+

#include <avr/io.h>
#include <avr/wdt.h>
#include "sleep.h"

enum class State { INIT, PRESSED_ONCE, RELEASED_ONCE, PRESSED_TWICE,
                   RELEASED_TWICE, PRESSED_THRICE, LIGHT_ON };

#define PORT_LED PORTA
constexpr uint8_t PORT_LED_NEG = PORTA0;
constexpr uint8_t PORT_LED_POS = PORTA1;

constexpr uint8_t BUTTON_BIT_MASK = _BV(PIND2);

enum class LedState : uint8_t {
    GREEN = _BV(PORT_LED_POS) & ~(_BV(PORT_LED_NEG)),
    OFF = 0
};

bool isButtonDown() {
    if ((PIND & BUTTON_BIT_MASK) != 0) {
        sleep(WDTO_15MS, SLEEP_MODE_PWR_DOWN);
        return (PIND & BUTTON_BIT_MASK) != 0;
    }
    return false;
}

bool isButtonUp() {
    if ((PIND & BUTTON_BIT_MASK) == 0) {
        sleep(WDTO_15MS, SLEEP_MODE_PWR_DOWN);
        return (PIND & BUTTON_BIT_MASK) == 0;
    }
    return false;
}

void setLedState(LedState state) {
    constexpr uint8_t led_mask = _BV(PORT_LED_POS) | _BV(PORT_LED_NEG); 
    PORT_LED = (static_cast<uint8_t>(state) & led_mask) | (PORT_LED & ~led_mask);
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
            setLedState(LedState::OFF);
            if (isButtonDown())
                curState = State::PRESSED_ONCE;
            break;
        case State::PRESSED_ONCE:
            if (isButtonUp())
                curState = State::RELEASED_ONCE;
            break;
        case State::RELEASED_ONCE:
            if (isButtonDown())
                curState = State::PRESSED_TWICE;
            break;
        case State::PRESSED_TWICE:
            if (isButtonUp())
                curState = State::RELEASED_TWICE;
            break;
        case State::RELEASED_TWICE:
            if (isButtonDown())
                curState = State::PRESSED_THRICE;
            break;
        case State::PRESSED_THRICE:
            if (isButtonUp())
                curState = State::LIGHT_ON;
            break;
        case State::LIGHT_ON:
            setLedState(LedState::GREEN);
            sleep(WDTO_2S, SLEEP_MODE_PWR_DOWN);
            curState = State::INIT;
            break;
        default:;
        }
    }
}