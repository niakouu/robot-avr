#define BAUD 2400
#include "Uart.h"

#include <avr/io.h>
#include <util/setbaud.h>

Uart::Uart()
    : emulatedFile_({.flags = __SWR | __SRD,
                     .put = Uart::putChar,
                     .get = Uart::getChar,
                     .udata = this}) {
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
}

Uart::~Uart() {
    // TODO
}

void Uart::transmit(uint8_t data) {
    while ((UCSR0A & _BV(UDRE0)) == 0)
        ;
    UDR0 = data;
}

uint8_t Uart::receive() {
    while ((UCSR0A & _BV(RXC0)) == 0)
        ;
    return UDR0;
}

int Uart::putChar(char data, FILE* stream) {
    if (stream == nullptr || stream->udata == nullptr)
        return -1;

    Uart* uart = reinterpret_cast<Uart*>(stream->udata);

    uart->transmit(data);

    return 0;
}

int Uart::getChar(FILE* stream) {
    if (stream == nullptr || stream->udata == nullptr)
        return -1;

    Uart* uart = reinterpret_cast<Uart*>(stream->udata);

    return uart->receive();
}

FILE* Uart::getEmulatedFile() {
    return &this->emulatedFile_;
}
