#include <avr/wdt.h>
#include <string.h>
#include "sleep.h"
#include "Memory24.h"
#include "Pin.h"
#include "Led.h"

constexpr uint16_t START_ADDR = 0x0;
constexpr uint8_t POLY_STR[] = "POLYTECHNIQUE MONTREAL\0";

int main() {
    BidirectionalLed led(Pin::Region::A, Pin::Id::P1, Pin::Region::A, Pin::Id::P0);
    Memory24CXXX mem;
    uint8_t data[sizeof(POLY_STR)];
    mem.write(START_ADDR, POLY_STR, sizeof(POLY_STR));
    rawSleep(WDTO_15MS, SLEEP_MODE_PWR_DOWN);
    mem.read(START_ADDR, data, sizeof(data));
    if (memcmp(data, POLY_STR, sizeof(POLY_STR)) == 0)
        led.setColor(BidirectionalLed::Color::GREEN);
    else
        led.setColor(BidirectionalLed::Color::RED);
}