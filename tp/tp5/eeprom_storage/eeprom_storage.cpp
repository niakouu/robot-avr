#define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <string.h>
#include "sleep.h"
#include "memory_24.h"
#include "Pin.h"
#include "Led.h"

constexpr uint16_t START_ADDR = 0x0;
constexpr uint8_t POLY_STR[] = "POLYTECHNIQUE MONTREAL\0";

// #define PORT_LED PORTA
// constexpr uint8_t PORT_LED_NEG = PORTA0;
// constexpr uint8_t PORT_LED_POS = PORTA1;
// constexpr uint8_t DATADIR_LED_NEG = DDA0;
// constexpr uint8_t DATADIR_LED_POS = DDA1;

// enum class LedState : uint8_t {
//     RED = _BV(PORT_LED_POS) & ~(_BV(PORT_LED_NEG)),
//     GREEN = _BV(PORT_LED_NEG) & ~(_BV(PORT_LED_POS)),
//     OFF = 0
// };

// void setLedState(LedState ledState) {
//     constexpr uint8_t led_mask = _BV(PORT_LED_POS) | _BV(PORT_LED_NEG);
//     PORT_LED =
//         (static_cast<uint8_t>(ledState) & led_mask) | (PORT_LED & ~led_mask);
// }

int main() {
    BidirectionalLed led(Pin::Region::A, Pin::Id::P1, Pin::Region::A, Pin::Id::P0);
    led.setColor(BidirectionalLed::Color::GREEN);
    rawSleep(WDTO_1S, SLEEP_MODE_PWR_DOWN);
    Memoire24CXXX mem;
    uint8_t data[sizeof(POLY_STR)];
    //mem.write(START_ADDR, POLY_STR, sizeof(POLY_STR));
    //rawSleep(WDTO_15MS, SLEEP_MODE_PWR_DOWN);
    mem.read(START_ADDR, data, sizeof(data));
    if (memcmp(data, POLY_STR, sizeof(POLY_STR)) == 0)
        led.setColor(BidirectionalLed::Color::GREEN);
    else
        led.setColor(BidirectionalLed::Color::RED);

    while (true);
}