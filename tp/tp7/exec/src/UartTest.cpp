#include "UartTest.h"

#include <stdio.h>

#include "Board.h"

const char* UartTest::NAME = "Uart";

const char* UartTest::getName() const {
    return UartTest::NAME;
}

uint8_t UartTest::runTests(void (*log)(const char* format, ...)) const {
    (void)log; // Log will use Uart, so don't use it.

    uint8_t fails = 0;
    Uart& uart = Board::get().getUart0();
    const char expectedChar = '1';
    const char testStringStatic[] = "Please send the following character ";
    const char testStringFormatted[] =
        "Please send the following character %c: ";

    for (const char i : testStringStatic)
        uart.transmit(i);

    uart.transmit(expectedChar);
    uart.transmit(':');
    uart.transmit(' ');

    if (uart.receive() != expectedChar)
        ++fails;

    FILE* emulatedFile = uart.getEmulatedFile();
    // Negate two because "%c" becomes "1" and NUL terminator removal
    if (fprintf(emulatedFile, testStringFormatted, expectedChar)
        != sizeof(testStringFormatted) - 2)
        ++fails;

    if (getc(emulatedFile) != expectedChar)
        ++fails;

    return fails;
}
