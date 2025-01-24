// 
// Travail : TP2
// Section # : 4
// Équipe # : 82
// Correcteur : CORRECTEUR
// ----------------------------------------------------------
// Développeurs : Edelina Alieva, Kyle Bouchard
// Description du pb2.cpp : 
// Identification matérielles : Button poussoir connecté à port D2 de l'Atmega324PA
//                              DEL côté positive connecté à broche A2
//                              DEL côté négative connecté à broche A1
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

constexpr uint8_t RED_LED = _BV(PORTA1) & ~(_BV(PORTA0));
constexpr uint8_t LED_GREEN = _BV(PORTA0) & ~(_BV(PORTA1));
constexpr uint8_t BUTTON_BIT_MASK = _BV(PIND2);
constexpr float SWITCH_DELAY = 7.5;

void waitForButtonPress() {
    while (!(PIND & BUTTON_BIT_MASK)) {
        sleep(WDTO_15MS, SLEEP_MODE_PWR_DOWN);
    }
}

void waitForButtonRelease() {
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
            PORTA = RED_LED;
            waitForButtonPress();
            curState = State::AMBER_COLOUR;
            break;
        case State::AMBER_COLOUR:
            while (PIND & BUTTON_BIT_MASK) {
                PORTA = LED_GREEN;
                _delay_ms(SWITCH_DELAY);
                PORTA = RED_LED;
                _delay_ms(SWITCH_DELAY);
            }
            curState = State::GREEN_COLOUR;
            break;
        case State::GREEN_COLOUR:
            PORTA = LED_GREEN;
            waitForButtonPress();
            curState = State::RED_COLOUR;
            break;
        case State::RED_COLOUR:
            PORTA = RED_LED;
            waitForButtonRelease();
            curState = State::LIGHT_OFF;
            break;
        case State::LIGHT_OFF:
            PORTA = 0;
            waitForButtonPress();
            curState = State::GREEN_COLOUR_2;
            break;
        case State::GREEN_COLOUR_2:
            PORTA = LED_GREEN;
            waitForButtonRelease();
            curState = State::INIT;
            break;
        default:;
        }
    }
}