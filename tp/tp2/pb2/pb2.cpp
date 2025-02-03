//
// Travail : TP2
// Section # : 4
// Équipe # : 82
// Correcteur : CORRECTEUR
// ----------------------------------------------------------
// Développeurs : Edelina Alieva, Kyle Bouchard
// Description du pb2.cpp : La DEL principale change de couleur à chaque clic et
// déclic
//                          du bouton, et après trois clics et déclics, le cycle
//                          recommence.
// Identification matérielles : Boutton poussoir connecté au port D2 de
// l'Atmega324PA (entré)
//                              DEL bidirectionelle sur la carte mère:
//                                  DEL côté positive connecté à la broche A2
//                                  (sortie) DEL côté négative connecté à la
//                                  broche A1 (sortie)
// Table des états :
// +----------------+---------+----------------+-----------+-------------+-------------+
// | Current State  | pressed | Next State     | color_red | color_green |
// color_amber |
// +----------------+---------+----------------+-----------+-------------+-------------+
// | INIT           | 0       | INIT           | 1         | 0           | 0 |
// | INIT           | 1       | AMBER_COLOUR   | 1         | 0           | 0 |
// | AMBER_COLOUR   | 0       | GREEN_COLOUR   | 0         | 0           | 1 |
// | AMBER_COLOUR   | 1       | AMBER_COLOUR   | 0         | 0           | 1 |
// | GREEN_COLOUR   | 0       | GREEN_COLOUR   | 0         | 1           | 0 |
// | GREEN_COLOUR   | 1       | RED_COLOUR     | 0         | 1           | 0 |
// | RED_COLOUR     | 0       | LIGHT_OFF      | 1         | 0           | 0 |
// | RED_COLOUR     | 1       | RED_COLOUR     | 1         | 0           | 0 |
// | LIGHT_OFF      | 0       | LIGHT_OFF      | 0         | 0           | 0 |
// | LIGHT_OFF      | 1       | GREEN_COLOUR_2 | 0         | 0           | 0 |
// | GREEN_COLOUR_2 | 0       | INIT           | 0         | 1           | 0 |
// | GREEN_COLOUR_2 | 1       | GREEN_COLOUR_2 | 0         | 1           | 0 |
// +----------------+---------+----------------+-----------+-------------+-------------+

#define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include "sleep.h"

#define PORT_LED PORTA
constexpr uint8_t PORT_LED_NEG = PORTA0;
constexpr uint8_t PORT_LED_POS = PORTA1;
constexpr uint8_t DATADIR_LED_NEG = DDA0;
constexpr uint8_t DATADIR_LED_POS = DDA1;

constexpr uint8_t BUTTON_STATE_BITMASK = _BV(PIND2);
constexpr float AMBER_SWITCH_DELAY = 7;

enum class State {
    INIT,
    AMBER_COLOUR,
    GREEN_COLOUR,
    RED_COLOUR,
    LIGHT_OFF,
    GREEN_COLOUR_2
};

enum class LedState : uint8_t {
    RED = _BV(PORT_LED_POS) & ~(_BV(PORT_LED_NEG)),
    GREEN = _BV(PORT_LED_NEG) & ~(_BV(PORT_LED_POS)),
    OFF = 0
};

bool isButtonDown() {
    if ((PIND & BUTTON_STATE_BITMASK) != 0) {
        sleep(WDTO_15MS, SLEEP_MODE_PWR_DOWN);
        return (PIND & BUTTON_STATE_BITMASK) != 0;
    }

    return false;
}

bool isButtonUp() {
    if ((PIND & BUTTON_STATE_BITMASK) == 0) {
        sleep(WDTO_15MS, SLEEP_MODE_PWR_DOWN);
        return (PIND & BUTTON_STATE_BITMASK) == 0;
    }

    return false;
}

void setLedState(LedState ledState) {
    constexpr uint8_t led_mask = _BV(PORT_LED_POS) | _BV(PORT_LED_NEG);
    PORT_LED =
        (static_cast<uint8_t>(ledState) & led_mask) | (PORT_LED & ~led_mask);
}

void executeAmberCycle() {
    setLedState(LedState::GREEN);
    _delay_ms(AMBER_SWITCH_DELAY);
    setLedState(LedState::RED);
    _delay_ms(AMBER_SWITCH_DELAY);
}

int main() {
    DDRA |= _BV(DATADIR_LED_NEG) | _BV(DATADIR_LED_POS);
    DDRD &= ~(_BV(DDD2));

    State curState = State::INIT;
    while (true) {
        switch (curState) {
            case State::INIT:
                setLedState(LedState::RED);
                if (isButtonDown())
                    curState = State::AMBER_COLOUR;
                break;
            case State::AMBER_COLOUR:
                executeAmberCycle();
                if (isButtonUp())
                    curState = State::GREEN_COLOUR;
                break;
            case State::GREEN_COLOUR:
                setLedState(LedState::GREEN);
                if (isButtonDown())
                    curState = State::RED_COLOUR;
                break;
            case State::RED_COLOUR:
                setLedState(LedState::RED);
                if (isButtonUp())
                    curState = State::LIGHT_OFF;
                break;
            case State::LIGHT_OFF:
                setLedState(LedState::OFF);
                if (isButtonDown())
                    curState = State::GREEN_COLOUR_2;
                break;
            case State::GREEN_COLOUR_2:
                setLedState(LedState::GREEN);
                if (isButtonUp())
                    curState = State::INIT;
                break;
            default:;
        }
    }
}