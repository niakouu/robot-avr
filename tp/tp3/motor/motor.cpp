#define F_CPU 8'000'000UL
#include <util/delay.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include "sleep.h"

#define PORT_MOTORS PORTD

constexpr uint8_t DELAY_US = 100;

constexpr uint8_t PORT_LEFT_EN = PORTD4;
constexpr uint8_t PORT_LEFT_DIR = PORTD5;
constexpr uint8_t PORT_RIGHT_EN = PORTD6;
constexpr uint8_t PORT_RIGHT_DIR = PORTD7;

constexpr uint8_t SPIN_TIME_SECS = 1;
constexpr uint16_t FREQS[] = {60, 400};
constexpr float RATIOS[] = {0.0f, .25f, .50f, .75f, 1.0f};

inline float secToMicro(float seconds) {
    return seconds * 1'000'000.0f;
}

inline float freqToSec(float freq) {
    return 1.0f / freq;
}

void spinMotor(uint16_t freq, float ratio, float duration)
{
    uint32_t cycleCount = static_cast<uint32_t>(duration * static_cast<float>(freq));
    uint16_t iterationsPerCycle = static_cast<uint16_t>(secToMicro(freqToSec(static_cast<float>(freq))) / DELAY_US);
    uint16_t onIterations = static_cast<uint16_t>(ratio * static_cast<float>(iterationsPerCycle));
    
    for (uint32_t i = 0; i < cycleCount; i++)
    {
        PORT_MOTORS |= _BV(PORT_LEFT_EN);
        for (uint16_t j = 0; j < onIterations; j++)
        {
            _delay_us(DELAY_US);
        }
        PORT_MOTORS &= ~(_BV(PORT_LEFT_EN));
        for (uint16_t j = 0; j < iterationsPerCycle - onIterations; j++)
        {
            _delay_us(DELAY_US);
        }
    }
}

int main() {
    DDRD |= _BV(PORT_LEFT_EN) | _BV(PORT_LEFT_DIR) | _BV(PORT_RIGHT_EN) | _BV(PORT_RIGHT_DIR);
    PORTD &= ~(_BV(PORT_LEFT_EN) | _BV(PORT_LEFT_DIR) | _BV(PORT_RIGHT_EN) | _BV(PORT_RIGHT_DIR));
    while (true)
    {
        for (uint16_t freq : FREQS)
        {
            for (float ratio : RATIOS)
            {
                spinMotor(freq, ratio, SPIN_TIME_SECS);
            }
            PORT_MOTORS &= ~(_BV(PORT_LEFT_EN));
            sleep(WDTO_1S, SLEEP_MODE_PWR_DOWN);
        }
        
    }
    
    return 0;
}