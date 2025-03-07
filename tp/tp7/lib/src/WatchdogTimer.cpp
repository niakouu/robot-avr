#include "WatchdogTimer.h"

#include <avr/sleep.h>
#include <avr/wdt.h>
#include <util/atomic.h>
#include <util/delay.h>

#ifndef F_CPU
#error F_CPU not defined
#endif

WatchdogTimer::WatchdogTimer() : sleepDone_(false) {}

WatchdogTimer::~WatchdogTimer() {
    wdt_disable();
}

void WatchdogTimer::sleep(const uint16_t milliseconds,
                          const SleepMode sleepMode) {

    uint16_t timeSlept = 0;
    while (timeSlept < milliseconds) {
        uint16_t diff = milliseconds - timeSlept;
        if (diff < 15) {
            for (uint8_t i = 0; i < diff; ++i) {
                _delay_ms(1);
            }

            timeSlept += milliseconds;
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

void WatchdogTimer::setSleepDone() {
    this->sleepDone_ = true;
}

#ifdef SIMULATION
void WatchdogTimer::rawSleep(const uint8_t durationMode,
                             const SleepMode sleepMode) {
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
#else  /* SIMULATION */
void WatchdogTimer::rawSleep(const uint8_t durationMode,
                             const SleepMode sleepMode) {
    uint8_t sregInitState =
        SREG; // Save Status Register if we are in interrupt handler.

    WatchdogTimer::wdtEnableInterrupt(durationMode);
    set_sleep_mode(static_cast<uint8_t>(sleepMode));
    sleep_enable();

    this->sleepDone_ = false;
    while (!this->sleepDone_)
        sleep_cpu();

    sleep_disable();      // cancel sleep as a precaution
    SREG = sregInitState; // Restore Status Register to previous state.
}
#endif /* SIMULATION */

void WatchdogTimer::wdtEnableInterrupt(const uint8_t durationMode) {
    uint8_t flags = _BV(WDIE) | (durationMode & 0x08 ? _BV(WDP3) : 0x00)
                    | (durationMode & 0x07);

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        MCUSR &= ~_BV(WDRF); // Turn off WDRF (required to turn off WDE)
        WDTCSR = _BV(WDCE) | _BV(WDE); // Start change operation
        WDTCSR = flags;
    }
}
