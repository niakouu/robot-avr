#ifndef _SLEEP_H
#define _SLEEP_H

#include <stdint.h>

/// @brief Sleep for a specific duration and a power saving mode.
/// @param duration Select from `avr/wdt.h`: One of the `WDTO_*` constants.
/// @param sleep_mode Select from `avr/io.h`: One of the `SLEEP_MODE_*` constants.
void sleep(const uint8_t duration, const uint8_t sleep_mode);

#endif /* _SLEEP_H */