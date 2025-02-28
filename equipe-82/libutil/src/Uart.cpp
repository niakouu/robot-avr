#define BAUD 2400
#define F_CPU 8'000'000UL
#include "Uart.h"

#include <avr/io.h>
#include <util/setbaud.h>

Uart::Uart() {
    // 2400 bauds
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

    // allow reception and transmission
    UCSR0A = _BV(UDRE0);
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);

    // 8 bits, 1 stop bits, parity none
    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00)
             | (UCSR0C
                & ~(_BV(UPM01) | _BV(UPM00) | _BV(USBS0) | _BV(UCSZ02)
                    | _BV(UCPOL0)));

    this->stdout_ = {.flags = 0x0002,
                     .put = Uart::putChar,
                     .get = __null,
                     .udata = this};
}

void Uart::transmit(char data) {
    while ((UCSR0A & _BV(UDRE0)) == 0)
    ;
    UDR0 = data;
}

int Uart::putChar(char data, FILE* stream) {
    if (!stream || !stream->udata)
        return -1;

    Uart *uart = reinterpret_cast<Uart *>(stream->udata);

    uart->transmit(data);

    return 0;
}

char Uart::receive() {
    return 0;
}

FILE* Uart::getStdout() {
    return &this->stdout_;
}
