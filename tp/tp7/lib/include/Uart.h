#ifndef _UART_H
#define _UART_H

#include <avr/io.h>
#include <stdio.h>

#include "common.h"

ASSERT_REGISTER_FLAGS_MATCH(UMSEL00, UMSEL10);
ASSERT_REGISTER_FLAGS_MATCH(UMSEL01, UMSEL11);

ASSERT_REGISTER_FLAGS_MATCH(UPM00, UPM10);
ASSERT_REGISTER_FLAGS_MATCH(UPM01, UPM11);

ASSERT_REGISTER_FLAGS_MATCH(USBS0, USBS1);
ASSERT_REGISTER_FLAGS_MATCH(UCPOL0, UCPOL1);

ASSERT_REGISTER_FLAGS_MATCH(UCSZ00, UCSZ10);
ASSERT_REGISTER_FLAGS_MATCH(UCSZ01, UCSZ11);
ASSERT_REGISTER_FLAGS_MATCH(UCSZ02, UCSZ12);

class Uart {
public:
    enum class Parity : uint8_t {
        DISABLED = 0,
        EVEN = _BV(UPM01),
        ODD = _BV(UPM01) | _BV(UPM00)
    };

    enum class StopBit : uint8_t { ONE_BIT = 0, TWO_BITS = _BV(USBS0) };

    struct Registers {
        volatile uint8_t *data, *controlStatusA, *controlStatusB,
            *controlStatusC;
        volatile uint16_t* baudRate;
    };

    Uart(Uart&) = delete;
    void operator=(const Uart&) = delete;

    void transmit(uint8_t data) const;
    uint8_t receive() const;
    FILE* getEmulatedFile() const;

    void configure(uint16_t baudRate, bool synchronous, Parity parity,
                   StopBit stopBit) const;

    void start(bool interruptsEnabled = false) const;
    void stop() const;

private:
    friend class Board;

    Uart(const Registers& registers);
    ~Uart();

    mutable FILE emulatedFile_;

    const Registers& registers_;

    void setBaudRate(uint16_t baudRate, bool synchronous) const;

    static int putChar(char data, FILE* stream);
    static int getChar(FILE* stream);
};

extern const Uart::Registers USART0_REGISTERS, USART1_REGISTERS;

#endif /* _UART_H */
