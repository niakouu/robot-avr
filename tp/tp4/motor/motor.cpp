#define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include "sleep.h"

#define PORT_MOTORS PORTD

constexpr uint8_t PORT_LEFT_EN = PORTD4;
constexpr uint8_t PORT_LEFT_DIR = PORTD6;
constexpr uint8_t PORT_RIGHT_EN = PORTD5;
constexpr uint8_t PORT_RIGHT_DIR = PORTD7;

constexpr float RATIOS[] = {0.0f, 0.25f, 0.5f, 0.75f, 1.0f};
constexpr uint8_t TOP = 0xFF;

void adjustPwmRatio(float ratio) {
    OCR1A = static_cast<uint16_t>(ratio * static_cast<float>(TOP));
    if (OCR1A >= TOP)
        OCR1A = TOP - 1;

    OCR1B = static_cast<uint16_t>(ratio * static_cast<float>(TOP));
    if (OCR1B >= TOP)
        OCR1B = TOP - 1;

    // clock division by 8 -> freq = 1960 Hz
    // mode #1 -> PWM 8 bit, correct phase
    TCCR1A = _BV(WGM10) | _BV(COM1A1) | _BV(COM1B1)
              | (TCCR1A & ~(_BV(WGM11) | _BV(COM1A0) | _BV(COM1B0)));

    TCCR1B = _BV(CS11)
              | (TCCR1B & ~(_BV(WGM13) | _BV(WGM12) | _BV(CS12) | _BV(CS10)));

    TCCR1C = 0;
}

void init() {
    DDRD |= _BV(PORT_LEFT_EN) | _BV(PORT_LEFT_DIR) | _BV(PORT_RIGHT_EN)
            | _BV(PORT_RIGHT_DIR);
    PORTD &= ~(_BV(PORT_LEFT_EN) | _BV(PORT_LEFT_DIR) | _BV(PORT_RIGHT_EN)
               | _BV(PORT_RIGHT_DIR));
}

int main() {
    init();
    while (true) {
        for (float ratio : RATIOS) {
            adjustPwmRatio(ratio);
            sleep(WDTO_2S, SLEEP_MODE_IDLE);
        }
    }
    return 0;
}