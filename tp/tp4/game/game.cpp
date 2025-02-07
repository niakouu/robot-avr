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

enum class State : uint8_t { INIT, BLINK, WAIT_BUTTON, FINISH };
enum class LedState : uint8_t {
    RED = _BV(PORT_LED_POS) & ~(_BV(PORT_LED_NEG)),
    GREEN = _BV(PORT_LED_NEG) & ~(_BV(PORT_LED_POS)),
    OFF = 0
};

volatile uint8_t g_timerExpired = 0;
volatile uint8_t g_buttonPressed = 0;

void setLedState(LedState ledState);
ISR(TIMER1_COMPA_vect) {
    g_timerExpired = 1;
}

ISR(INT0_vect) {
    sleep(WDTO_15MS, SLEEP_MODE_IDLE);
    g_buttonPressed |= (PIND & BUTTON_STATE_BITMASK) != 0;
}

void setLedState(LedState ledState) {
    constexpr uint8_t led_mask = _BV(PORT_LED_POS) | _BV(PORT_LED_NEG);
    PORT_LED =
        (static_cast<uint8_t>(ledState) & led_mask) | (PORT_LED & ~led_mask);
}

void startTimer(uint16_t time) {
    g_timerExpired = 0;
    // mode CTC timer 1 with clock 1024
    TCNT1 = 0;
    OCR1A = time;
    TCCR1A = _BV(COM1A1) | (TCCR1A & ~_BV(COM1A0));
    TCCR1B = _BV(CS12) | _BV(CS10) | (TCCR1B & ~_BV(CS11));
    TCCR1C = 0;
    TIMSK1 |= _BV(OCIE1A);
}

void initialization() {
    cli();

    DDRA |= _BV(DATADIR_LED_NEG) | _BV(DATADIR_LED_POS);
    DDRD &= ~(_BV(DDD2));

    EIMSK |= _BV(INT0); // activate interrupt 0

    // ISR called on every edge
    EICRA &= ~(_BV(ISC01));
    EICRA |= _BV(ISC00);

    sei();
}

int main() {
    initialization();

    setLedState(LedState::OFF);
    _delay_ms(10000);

    g_buttonPressed = 0;
    startTimer(7812);
    do {
        setLedState(LedState::RED);
        sleep(WDTO_30MS, SLEEP_MODE_IDLE);
        setLedState(LedState::OFF);
        sleep(WDTO_30MS, SLEEP_MODE_IDLE);
    } while (g_timerExpired == 0 && g_buttonPressed == 0);

    cli();

    if (g_buttonPressed && !g_timerExpired)
        setLedState(LedState::GREEN);
    else
        setLedState(LedState::RED);

    while (true) { sleep(WDTO_8S, SLEEP_MODE_PWR_DOWN); }

    return 0;
}