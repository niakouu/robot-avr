#define F_CPU 8000000UL
#include <util/delay.h>
#include <avr/io.h>

#define PORT_LED PORTA
constexpr uint8_t PORT_LED_NEG = PORTA0;
constexpr uint8_t PORT_LED_POS = PORTA1;

constexpr uint8_t BUTTON_STATE_BITMASK = _BV(PIND2);
constexpr float AMBER_SWITCH_DELAY = 7;

enum class LedState : uint8_t
{
    RED = _BV(PORT_LED_POS) & ~(_BV(PORT_LED_NEG)),
    GREEN = _BV(PORT_LED_NEG) & ~(_BV(PORT_LED_POS)),
    OFF = 0
};

constexpr uint16_t PRECISION = 1000;
constexpr float MAX_RATIO = 1.0f;
constexpr float MIN_RATIO = 0.0f;
constexpr float RATIO_DELTA = 1.0f/1000.0f;

void setLedState(LedState state)
{
    constexpr uint8_t led_mask = _BV(PORT_LED_POS) | _BV(PORT_LED_NEG);
    PORT_LED = (static_cast<uint8_t>(state) & led_mask) | (PORT_LED & ~led_mask);
}

void powerOffLed(LedState color)
{
    float ratio = MAX_RATIO;
    while (ratio >= MIN_RATIO)
    {
        uint16_t onCycles = static_cast<uint16_t>(ratio * static_cast<float>(PRECISION));
        setLedState(color);
        for (uint16_t i = 0; i < onCycles; i++)
        {
            _delay_us(1);
        }
        setLedState(LedState::OFF);
        for (uint16_t i = 0; i < PRECISION - onCycles; i++)
        {
            _delay_us(1);
        }
        ratio -= 1.0f/1000.0f;
    }
}

int main(void)
{
    DDRD &= ~_BV(DDD2);
    DDRA |= _BV(DDA0) | _BV(DDA1);

    while (true)
    {
        powerOffLed(LedState::RED);
        powerOffLed(LedState::GREEN);
    }

    return 0;
}