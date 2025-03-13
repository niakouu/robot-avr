#include "WatchdogTimer.h"

#include <avr/sleep.h>
#include <avr/wdt.h>
#include <util/atomic.h>
#include <util/delay.h>

WatchdogTimer::WatchdogTimer() : sleepDone_(false) {}

WatchdogTimer::~WatchdogTimer() {
    wdt_disable();
}

void WatchdogTimer::sleep(const uint16_t milliseconds,
                          const SleepMode sleepMode) {
    constexpr static struct {
        uint8_t wdtoFlag;
        uint16_t timeMs;
    } TIME_BOUNDARIES[]{{WDTO_15MS, 15},   {WDTO_30MS, 30},   {WDTO_60MS, 60},
                        {WDTO_120MS, 120}, {WDTO_250MS, 250}, {WDTO_500MS, 500},
                        {WDTO_1S, 1000},   {WDTO_2S, 2000},   {WDTO_4S, 4000},
                        {WDTO_8S, 8000}};

    uint16_t timeLeft = milliseconds;
    while (timeLeft != 0) {
        if (timeLeft < TIME_BOUNDARIES[0].timeMs) {
            static_assert(TIME_BOUNDARIES[0].timeMs <= UINT8_MAX,
                          "First TIME_BOUNDARIES element should fit in a byte");
            for (uint8_t i = 0; i < static_cast<uint8_t>(timeLeft); ++i) {
                _delay_ms(1);
            }

            timeLeft = 0;
        } else {
            static_assert(sizeof(TIME_BOUNDARIES) / sizeof(*TIME_BOUNDARIES)
                              <= INT8_MAX,
                          "Length of TIME_BOUNDARIES should fit in int8_t");
            constexpr int8_t TIME_BOUNDARIES_LEN = static_cast<int8_t>(
                sizeof(TIME_BOUNDARIES) / sizeof(*TIME_BOUNDARIES));
            for (int8_t i = TIME_BOUNDARIES_LEN - 1; i >= 0; --i) {
                const uint16_t timeBoundaryMs = TIME_BOUNDARIES[i].timeMs;
                if (timeLeft >= timeBoundaryMs) {
                    timeLeft -= timeBoundaryMs;
                    rawSleep(TIME_BOUNDARIES[i].wdtoFlag, sleepMode);
                    break;
                }
            }
        }
    }
}

void WatchdogTimer::setSleepDone() {
    this->sleepDone_ = true;
}

#ifdef SIMULATION

void WatchdogTimer::rawSleep(const uint8_t durationMode,
                             const SleepMode sleepMode) {
    switch (durationMode) {
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

#else /* SIMULATION */

void WatchdogTimer::rawSleep(const uint8_t durationMode,
                             const SleepMode sleepMode) {
    WatchdogTimer::wdtEnableInterrupt(durationMode);
    set_sleep_mode(static_cast<uint8_t>(sleepMode));
    sleep_enable();

    this->sleepDone_ = false;
    while (!this->sleepDone_)
        sleep_cpu();

    sleep_disable(); // cancel sleep as a precaution
}

#endif /* SIMULATION */

void WatchdogTimer::wdtEnableInterrupt(const uint8_t durationMode) {
    constexpr uint8_t SLEEP_MODE_WDP3_TOGGLE_BIT = 0x08;
    const uint8_t flags =
        _BV(WDIE)
        | ((durationMode & SLEEP_MODE_WDP3_TOGGLE_BIT) != 0 ? _BV(WDP3) : 0x00)
        | (durationMode & ~SLEEP_MODE_WDP3_TOGGLE_BIT);

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        MCUSR &= ~_BV(WDRF); // Turn off WDRF (required to turn off WDE)
        WDTCSR = _BV(WDCE) | _BV(WDE); // Start change operation
        WDTCSR = flags;
    }
}
