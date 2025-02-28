#define F_CPU 8000000
#include <avr/io.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include "sleep.h"

typedef enum states { STATE_RED, STATE_GREEN, STATE_AMBER, STATE_MAX } states_t;

#define SWITCH_DELAY 7.5
#define CYCLE_COUNT 66.66

int main(void) {
    DDRD &= ~_BV(DDD2);
    DDRA |= _BV(DDA0) | _BV(DDA1);

    states_t state = STATE_RED;
    uint8_t buttonClicked = 0;
    uint8_t cycles = 0;

    for (;;) {
        switch (state) {
            case STATE_RED:
                PORTA = _BV(PORTA0);
                sleep(15, SLEEP_MODE_IDLE);
                break;
            case STATE_GREEN:
                PORTA = _BV(PORTA1);
                sleep(15, SLEEP_MODE_IDLE);
                break;
            case STATE_AMBER:
                PORTA = _BV(PORTA0);
                _delay_ms(SWITCH_DELAY);
                PORTA = _BV(PORTA1);
                _delay_ms(SWITCH_DELAY);
                break;
            case STATE_MAX:
            default:;
        }

        if (!buttonClicked) {
            if (PIND & _BV(PIND2)) {
                rawSleep(WDTO_15MS, SLEEP_MODE_IDLE);
                buttonClicked = (PIND & _BV(PIND2)) != 0;
            } else if (cycles++ > CYCLE_COUNT) {
                cycles = 0;
                state = (states_t)((state + 1) % STATE_MAX);
            }
        } else if (!(PIND & _BV(PIND2))) {
            PORTA = 0;
            break;
        }
    }

    return 0;
}