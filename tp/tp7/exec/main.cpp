#include <stdio.h>

#include "Board.h"
#include "LedTest.h"
#include "Test.h"
#include "UartTest.h"

namespace {
    UartTest UART_TEST{};
    LedTest LED_TEST{};
    constexpr uint16_t BAUD_RATE = 2400;

    void testLog(const char* name, const char* format, ...) {
        va_list va = nullptr;
        va_start(va, format);
        printf("  [%s]: LOG: ", name);
        vprintf(format, va);
        va_end(va);
    }
} // namespace

int main() {
    const Test* tests[] = {&::UART_TEST, &::LED_TEST};
    Uart& uart0 = Board::get().getUart0();
    stdout = uart0.getEmulatedFile();
    uart0.configure(::BAUD_RATE, false, Uart::Parity::DISABLED,
                    Uart::StopBit::ONE_BIT);
    uart0.start();

    // Assume usart is working
    puts("Tests started");

    uint16_t totalFails = 0;
    for (const Test* test : tests) {
        const uint8_t fails = test->runTests(::testLog);
        totalFails += fails;

        printf("  [%s]: ", test->getName());
        if (fails == 0)
            puts("Success\n");
        else
            printf("%d failures!\n", fails);
    }

    printf("Tests completed ");
    if (totalFails == 0)
        puts("successfully!");
    else
        printf("with %d failures\n", totalFails);

    return 0;
}