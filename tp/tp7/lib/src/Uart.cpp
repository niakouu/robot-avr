#include "Uart.h"

#include <avr/io.h>
#include <util/atomic.h>

const Uart::Registers USART0_REGISTERS{.data = &UDR0,
                                       .controlStatusA = &UCSR0A,
                                       .controlStatusB = &UCSR0B,
                                       .controlStatusC = &UCSR0C,
                                       .baudRate = &UBRR0};

const Uart::Registers USART1_REGISTERS{.data = &UDR1,
                                       .controlStatusA = &UCSR1A,
                                       .controlStatusB = &UCSR1B,
                                       .controlStatusC = &UCSR1C,
                                       .baudRate = &UBRR1};

Uart::Uart(const Registers& registers)
    : emulatedFile_({.flags = __SWR | __SRD,
                     .put = Uart::putChar,
                     .get = Uart::getChar,
                     .udata = this}),
      registers_(registers) {
    this->stop();
}

void Uart::configure(uint16_t baudRate, bool synchronous, Parity parity,
                     StopBit stopBit) const {
    uint8_t syncBit = synchronous ? _BV(UMSEL00) : 0;
    uint8_t characterSize = _BV(UCSZ01) | _BV(UCSZ00);

    this->setBaudRate(baudRate, synchronous);

    *this->registers_.controlStatusB &= ~_BV(UCSZ02);
    *this->registers_.controlStatusC = syncBit | static_cast<uint8_t>(parity)
                                       | static_cast<uint8_t>(stopBit)
                                       | characterSize;
}

void Uart::start(bool interruptsEnabled) const {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        *this->registers_.controlStatusA = _BV(UDRE0);
        *this->registers_.controlStatusB |= _BV(TXEN0) | _BV(RXEN0);

        if (interruptsEnabled)
            *this->registers_.controlStatusB |= _BV(TXCIE0) | _BV(TXCIE1);
        else
            *this->registers_.controlStatusB &= ~(_BV(TXCIE0) | _BV(TXCIE1));
    }
}

void Uart::stop() const {
    *this->registers_.controlStatusB &= ~(_BV(TXEN0) | _BV(RXEN0));
    *this->registers_.controlStatusC &= ~(_BV(UPM00) | _BV(UPM01));
}

Uart::~Uart() {
    this->stop();
}

void Uart::transmit(uint8_t data) const {
    while ((UCSR0A & _BV(UDRE0)) == 0)
        ;
    UDR0 = data;
}

uint8_t Uart::receive() const {
    while ((UCSR0A & _BV(RXC0)) == 0)
        ;
    return UDR0;
}

int Uart::putChar(char data, FILE* stream) {
    if (stream == nullptr || stream->udata == nullptr)
        return -1;

    const Uart* uart = reinterpret_cast<const Uart*>(stream->udata);

    uart->transmit(data);

    return 0;
}

void Uart::setBaudRate(uint16_t baudRate, bool synchronous) const {
    uint8_t divisionFactor = synchronous ? 2 : 16;

    *this->registers_.baudRate = (F_CPU / divisionFactor) / baudRate - 1;
}

int Uart::getChar(FILE* stream) {
    if (stream == nullptr || stream->udata == nullptr)
        return -1;

    Uart* uart = reinterpret_cast<Uart*>(stream->udata);

    return uart->receive();
}

FILE* Uart::getEmulatedFile() const {
    return &this->emulatedFile_;
}
