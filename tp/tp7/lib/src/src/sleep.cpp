#include "sleep.h"

#define F_CPU 8000000UL
#include <avr/wdt.h>
#include <util/delay.h>

#ifdef SIMULATION

void rawSleep(const uint8_t duration, const uint8_t sleepMode) {
    switch (duration) {
        case WDTO_15MS:
            _delay_ms(15);
            break;
        case WDTO_30MS:
            _delay_ms(30);
            break;
        case WDTO_60MS:
            _delay_ms(60);
            break;
        case WDTO_120MS:
            _delay_ms(120);
            break;
        case WDTO_250MS:
            _delay_ms(250);
            break;
        case WDTO_500MS:
            _delay_ms(500);
            break;
        case WDTO_1S:
            _delay_ms(1000);
            break;
        case WDTO_2S:
            _delay_ms(2000);
            break;
        case WDTO_4S:
            _delay_ms(4000);
            break;
        case WDTO_8S:
            _delay_ms(8000);
            break;
        default:
            break;
    }
}

#else
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>

volatile bool g_sleepDone = false;

/// @brief WDT interrupt handler. Disables WDT to continue execution from last
/// point.
ISR(WDT_vect) {
    wdt_disable();
    ::g_sleepDone = true;
}

/// @brief Enables the WDT timer on interrupt mode with a specific timer.
/// @param duration Duration to enable the WDT timer for. Select from
/// `avr/wdt.h`: One of the `WDTO_*` constants.
static void wdt_enable_interrupt(const uint8_t duration) {
    uint8_t flags =
        _BV(WDIE) | (duration & 0x08 ? _BV(WDP3) : 0x00) | (duration & 0x07);
    cli();               // Disable interrupts before time critical section
    MCUSR &= ~_BV(WDRF); // Turn off WDRF (required to turn off WDE)
    WDTCSR = _BV(WDCE) | _BV(WDE); // Start change operation
    WDTCSR = flags;
    sei(); // Enable interrupts after time critical section
}

void rawSleep(const uint8_t duration, const uint8_t sleepMode) {
    uint8_t sregInitState =
        SREG; // Save Status Register if we are in interrupt handler.
    wdt_enable_interrupt(duration);
    set_sleep_mode(sleepMode);
    sleep_enable();

    ::g_sleepDone = false;
    while (!::g_sleepDone)
        sleep_cpu();

    sleep_disable();      // cancel sleep as a precaution
    SREG = sregInitState; // Restore Status Register to previous state.
}
#endif /* SIMULATION */

void sleep(const uint16_t millis, const uint8_t sleepMode) {
    uint16_t timeSlept = 0;
    while (timeSlept < millis) {
        uint16_t diff = millis - timeSlept;
        if (diff < 15) {
            for (uint8_t i = 0; i < diff; ++i) {
                _delay_ms(1);
            }

            timeSlept += millis;
        } else if (diff < 30) {
            rawSleep(WDTO_15MS, sleepMode);
            timeSlept += 15;
        } else if (diff < 60) {
            rawSleep(WDTO_30MS, sleepMode);
            timeSlept += 30;
        } else if (diff < 120) {
            rawSleep(WDTO_60MS, sleepMode);
            timeSlept += 60;
        } else if (diff < 250) {
            rawSleep(WDTO_120MS, sleepMode);
            timeSlept += 120;
        } else if (diff < 500) {
            rawSleep(WDTO_250MS, sleepMode);
            timeSlept += 250;
        } else if (diff < 1000) {
            rawSleep(WDTO_500MS, sleepMode);
            timeSlept += 500;
        } else if (diff < 2000) {
            rawSleep(WDTO_1S, sleepMode);
            timeSlept += 1000;
        } else if (diff < 4000) {
            rawSleep(WDTO_2S, sleepMode);
            timeSlept += 2000;
        } else if (diff < 8000) {
            rawSleep(WDTO_4S, sleepMode);
            timeSlept += 4000;
        } else {
            rawSleep(WDTO_8S, sleepMode);
            timeSlept += 8000;
        }
    }
}
