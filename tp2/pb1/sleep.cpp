#include "sleep.h"
#include <avr/wdt.h>

#ifdef SIMULATION
#define F_CPU 8000000
#include <util/delay.h>

void sleep(const uint8_t duration, const uint8_t sleep_mode) {
    switch (duration)
    {
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
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/power.h>

ISR (WDT_vect)  {
   wdt_disable();
}

void wdt_enable_fix(const uint8_t value) {
    uint8_t flags = _BV(WDIE) | (value & 0x08 ? _BV(WDP3) : 0x00) | (value & 0x07);
    cli(); // Disable interrupts before time critical section
    MCUSR &= ~_BV(WDRF); // Turn off WDRF (required to turn off WDE)
    WDTCSR = _BV(WDCE) | _BV(WDE); // Start change operation
    WDTCSR = flags;
    sei(); // Enable interrupts after time critical section
}

void sleep(const uint8_t duration, const uint8_t sleep_mode) {
    wdt_enable_fix(duration);
    set_sleep_mode (sleep_mode);
    sleep_enable();
    sleep_cpu ();  
    sleep_disable(); // cancel sleep as a precaution
}
#endif /* SIMULATION */