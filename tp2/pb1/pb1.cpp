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
const uint8_t BUTTON_BIT_MASK = _BV(PIND2);

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
            PORTA = 0;
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
            PORTA = _BV(PORTA1) & ~(_BV(PORTA0));
            sleep(WDTO_2S, SLEEP_MODE_PWR_DOWN);
            curState = State::INIT;
            break;
        default:;
        }
    }
}