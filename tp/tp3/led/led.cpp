#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>

#define PORT_LED PORTA

constexpr uint8_t DELAY_US = 100;

constexpr uint8_t PORT_LED_NEG = PORTA0;
constexpr uint8_t PORT_LED_POS = PORTA1;

constexpr uint8_t LED_ON_DURATION_SEC = 3;
constexpr uint16_t FREQ = 1000;

enum class LedState : uint8_t {
    RED = _BV(PORT_LED_POS) & ~(_BV(PORT_LED_NEG)),
    GREEN = _BV(PORT_LED_NEG) & ~(_BV(PORT_LED_POS)),
    OFF = 0
};

inline float secToMicro(float seconds) { return seconds * 1'000'000.0f; }

inline float freqToSec(float freq) { return 1.0f / freq; }

void setLedState(LedState state) {
    constexpr uint8_t led_mask = _BV(PORT_LED_POS) | _BV(PORT_LED_NEG);
    PORT_LED =
        (static_cast<uint8_t>(state) & led_mask) | (PORT_LED & ~led_mask);
}

void powerOffLed(LedState color, uint16_t freq, float duration) {
    uint32_t cycleCount =
        static_cast<uint32_t>(duration * static_cast<float>(freq));
    uint16_t iterationsPerCycle = static_cast<uint16_t>(
        secToMicro(freqToSec(static_cast<float>(freq))) / DELAY_US);

    uint16_t onIterations;
    for (uint32_t i = 0; i < cycleCount; i++) {
        onIterations = static_cast<uint16_t>(
            static_cast<float>(i) / static_cast<float>(cycleCount)
            * static_cast<float>(iterationsPerCycle));

        setLedState(color);
        for (uint16_t j = 0; j < onIterations; j++) {
            _delay_us(DELAY_US);
        }
        setLedState(LedState::OFF);
        for (uint16_t j = 0; j < iterationsPerCycle - onIterations; j++) {
            _delay_us(DELAY_US);
        }
    }
}

int main(void) {
    DDRD &= ~_BV(DDD2);
    DDRA |= _BV(DDA0) | _BV(DDA1);

    while (true) {
        powerOffLed(LedState::RED, FREQ, LED_ON_DURATION_SEC);
        powerOffLed(LedState::GREEN, FREQ, LED_ON_DURATION_SEC);
    }

    return 0;
}