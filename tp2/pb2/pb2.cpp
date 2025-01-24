// 
// Travail : TP2
// Section # : 4
// Équipe # : 82
// Correcteur : CORRECTEUR
// ----------------------------------------------------------
// Développeurs : Edelina Alieva, Kyle Bouchard
// Description du pb2.cpp : La DEL principale change de couleur à chaque clic et déclic
//                          du bouton, et après trois clics et déclics, le cycle recommence.
// Identification matérielles : Boutton poussoir connecté au port D2 de l'Atmega324PA
//                              DEL bidirectionelle sur la carte mère:
//                                  DEL côté positive connecté à la broche A2
//                                  DEL côté négative connecté à la broche A1
// Table des états :
// +----------------+---------+----------------+-----------+-------------+-------------+
// | Current State  | pressed | Next State     | color_red | color_green | color_amber |
// +----------------+---------+----------------+-----------+-------------+-------------+
// | INIT           | 0       | INIT           | 1         | 0           | 0           |
// | INIT           | 1       | AMBER_COLOUR   | 1         | 0           | 0           |
// | AMBER_COLOUR   | 0       | GREEN_COLOUR   | 0         | 0           | 1           |
// | AMBER_COLOUR   | 1       | AMBER_COLOUR   | 0         | 0           | 1           |
// | GREEN_COLOUR   | 0       | GREEN_COLOUR   | 0         | 1           | 0           |
// | GREEN_COLOUR   | 1       | RED_COLOUR     | 0         | 1           | 0           |
// | RED_COLOUR     | 0       | LIGHT_OFF      | 1         | 0           | 0           |
// | RED_COLOUR     | 1       | RED_COLOUR     | 1         | 0           | 0           |
// | LIGHT_OFF      | 0       | LIGHT_OFF      | 0         | 0           | 0           |
// | LIGHT_OFF      | 1       | GREEN_COLOUR_2 | 0         | 0           | 0           |
// | GREEN_COLOUR_2 | 0       | INIT           | 0         | 1           | 0           |
// | GREEN_COLOUR_2 | 1       | GREEN_COLOUR_2 | 0         | 1           | 0           |
// +----------------+---------+----------------+-----------+-------------+-------------+

#define F_CPU 8000000
#include <avr/io.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include "sleep.h"

enum class State { INIT, AMBER_COLOUR, GREEN_COLOUR, RED_COLOUR, LIGHT_OFF, GREEN_COLOUR_2 };

constexpr uint8_t LED_RED = _BV(PORTA1) & ~(_BV(PORTA0));
constexpr uint8_t LED_GREEN = _BV(PORTA0) & ~(_BV(PORTA1));
constexpr uint8_t LED_OFF = 0;
constexpr uint8_t BUTTON_BIT_MASK = _BV(PIND2);
constexpr float SWITCH_DELAY = 7.5;

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

void executeAmberCycle() {
    PORTA = LED_GREEN;
    _delay_ms(SWITCH_DELAY);
    PORTA = LED_RED;
    _delay_ms(SWITCH_DELAY);
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
            PORTA = LED_RED;
            if (isButtonDown())
                curState = State::AMBER_COLOUR;
            break;
        case State::AMBER_COLOUR:
            executeAmberCycle();
            if (isButtonUp())
                curState = State::GREEN_COLOUR;
            break;
        case State::GREEN_COLOUR:
            PORTA = LED_GREEN;
            if (isButtonDown())
                curState = State::RED_COLOUR;
            break;
        case State::RED_COLOUR:
            PORTA = LED_RED;
            if (isButtonUp())
                curState = State::LIGHT_OFF;
            break;
        case State::LIGHT_OFF:
            PORTA = LED_OFF;
            if (isButtonDown())
                curState = State::GREEN_COLOUR_2;
            break;
        case State::GREEN_COLOUR_2:
            PORTA = LED_GREEN;
            if (isButtonUp())
                curState = State::INIT;
            break;
        default:;
        }
    }
}