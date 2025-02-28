#ifndef _SLEEP_H
#define _SLEEP_H

#include <stdint.h>

/// @brief Sleep for a predefined duration and a power saving mode.
/// @param duration Select from `avr/wdt.h`: One of the `WDTO_*` constants.
/// @param sleep_mode Select from `avr/io.h`: One of the `SLEEP_MODE_*`
/// constants.
void rawSleep(const uint8_t duration, const uint8_t sleepMode);

/// @brief Sleep for a specific duration and a power saving mode.
/// @param duration Duration of sleep in milliseconds
/// @param sleep_mode Select from `avr/io.h`: One of the `SLEEP_MODE_*`
/// constants.
void sleep(const uint16_t millis, const uint8_t sleepMode);

#endif /* _SLEEP_H */