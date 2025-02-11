#define F_CPU 8000000UL
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include "sleep.h"

#define PORT_LED PORTA
#define PIN_BTN PIND
constexpr uint8_t PORT_LED_NEG = PORTA0;
constexpr uint8_t PORT_LED_POS = PORTA1;
constexpr uint8_t DATADIR_LED_NEG = DDA0;
constexpr uint8_t DATADIR_LED_POS = DDA1;

constexpr uint8_t BUTTON_STATE_BITMASK = _BV(PIND2);
constexpr float AMBER_SWITCH_DELAY = 7.0f;

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

volatile bool g_buttonPressed = false;
volatile State g_curState = State::INIT;

void setLedState(LedState ledState) {
    constexpr uint8_t led_mask = _BV(PORT_LED_POS) | _BV(PORT_LED_NEG);
    PORT_LED =
        (static_cast<uint8_t>(ledState) & led_mask) | (PORT_LED & ~led_mask);
}

void executeAmberCycle(void) {
    setLedState(LedState::GREEN);
    _delay_ms(AMBER_SWITCH_DELAY);
    setLedState(LedState::RED);
    _delay_ms(AMBER_SWITCH_DELAY);
}

void handleButtonEvent(void) {
    switch (::g_curState) {
        case State::INIT:
            if (::g_buttonPressed)
                ::g_curState = State::AMBER_COLOUR;
            break;
        case State::AMBER_COLOUR:
            if (!::g_buttonPressed)
                ::g_curState = State::GREEN_COLOUR;
            break;
        case State::GREEN_COLOUR:
            if (::g_buttonPressed)
                ::g_curState = State::RED_COLOUR;
            break;
        case State::RED_COLOUR:
            if (!::g_buttonPressed)
                ::g_curState = State::LIGHT_OFF;
            break;
        case State::LIGHT_OFF:
            if (::g_buttonPressed)
                ::g_curState = State::GREEN_COLOUR_2;
            break;
        case State::GREEN_COLOUR_2:
            if (!::g_buttonPressed)
                ::g_curState = State::INIT;
            break;
        default:;
    }
}

void updateStateLed(void) {
    switch (::g_curState) {
        case State::INIT:
        case State::RED_COLOUR:
            setLedState(LedState::RED);
            break;
        case State::GREEN_COLOUR:
        case State::GREEN_COLOUR_2:
            setLedState(LedState::GREEN);
            break;
        case State::LIGHT_OFF:
            setLedState(LedState::OFF);
            break;
        case State::AMBER_COLOUR:
        default:;
    }
}

ISR(INT0_vect) {
    rawSleep(WDTO_30MS, SLEEP_MODE_PWR_DOWN); // anti-rebound

    ::g_buttonPressed = (PIN_BTN & BUTTON_STATE_BITMASK) != 0;
    handleButtonEvent();
    updateStateLed();

    EIFR |= _BV(INTF0); // activate interrupt 0
}

void initialization(void) {
    cli();

    DDRA |= _BV(DATADIR_LED_NEG) | _BV(DATADIR_LED_POS);
    DDRD &= ~(_BV(DDD2));

    EIMSK |= _BV(INT0); // activate interrupt 0

    // ISR called on every edge
    EICRA &= ~(_BV(ISC01));
    EICRA |= _BV(ISC00);

    updateStateLed();

    sei();
}

int main() {
    initialization();
    while (true) {
        cli();
        if (::g_curState == State::AMBER_COLOUR) {
            executeAmberCycle();
            sei();
        } else {
            sei();
            rawSleep(WDTO_120MS, SLEEP_MODE_PWR_DOWN);
        }
    }
    return 0;
}