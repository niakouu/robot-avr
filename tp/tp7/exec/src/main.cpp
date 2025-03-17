#include <avr/interrupt.h>
#include <stdio.h>

#include "Board.h"
#include "ButtonTest.h"
#include "LedTest.h"
#include "MemoryTest.h"
#include "MovementManagerTest.h"
#include "PhotoresistanceTest.h"
#include "Test.h"
#include "TimerCounterTest.h"
#include "TimerFrequencyTest.h"
#include "TimerPwmTest.h"
#include "UartTest.h"
#include "WatchdogTimerTest.h"

namespace {
    UartTest UART_TEST;
    LedTest LED_TEST;
    MemoryTest MEMORY_TEST;
    ButtonTest BUTTON_TEST;
    PhotoresistanceTest PHOTORESISTANCE_TEST;
    TimerCounterTest TIMER_COUNTER_TEST;
    TimerPwmTest TIMER_PWM_TEST;
    TimerFrequencyTest TIMER_FREQUENCY_TEST;
    WatchdogTimerTest WATCHDOG_TIMER_TEST;
    MovementManagerTest MOVEMENT_MANAGER_TEST;
    constexpr uint16_t BAUD_RATE = 2400;

    const char* gCurrentTestName = NULL;

    void testLog(const char* format, ...) {
        va_list va = nullptr;
        va_start(va, format);
        printf("  [%s]: LOG: ", ::gCurrentTestName);
        vprintf(format, va);
        va_end(va);
    }
} // namespace

ISR(INT0_vect) {
    Board::get().getUart0().transmit('9');
    Board::get().getButton().setPressed();
}

ISR(WDT_vect) {
    Board::get().getWatchdogTimer().setSleepDone();
}

int main() {
    const Test* tests[] = {&::UART_TEST,
                           &::LED_TEST,
                           &::MEMORY_TEST,
                           &::BUTTON_TEST,
                           &::PHOTORESISTANCE_TEST,
                           &::TIMER_COUNTER_TEST,
                           &::TIMER_PWM_TEST,
                           &::TIMER_FREQUENCY_TEST,
                           &::WATCHDOG_TIMER_TEST,
                           &::MOVEMENT_MANAGER_TEST};

    Uart& uart0 = Board::get().getUart0();
    stdout = uart0.getEmulatedFile();
    uart0.configure(::BAUD_RATE, false, Uart::Parity::DISABLED,
                    Uart::StopBit::ONE_BIT);
    uart0.start();

    // Assume usart is working
    puts("Tests started");

    uint16_t totalFails = 0;
    for (const Test* test : tests) {
        ::gCurrentTestName = test->getName();
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