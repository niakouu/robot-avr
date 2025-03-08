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
    : emulatedFile_{.flags = __SWR | __SRD,
                    .put = Uart::putChar,
                    .get = Uart::getChar,
                    .udata = this},
      registers_(registers) {
    this->stop();
}

void Uart::configure(uint16_t baudRate, bool synchronous, Parity parity,
                     StopBit stopBit) const {
    const uint8_t syncBit = synchronous ? _BV(UMSEL00) : 0;
    const uint8_t characterSize = _BV(UCSZ01) | _BV(UCSZ00);

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
            *this->registers_.controlStatusB |= _BV(TXCIE0) | _BV(RXCIE0);
        else
            *this->registers_.controlStatusB &= ~(_BV(TXCIE0) | _BV(RXCIE0));
    }
}

void Uart::stop() const {
    *this->registers_.controlStatusB &= ~(_BV(TXEN0) | _BV(RXEN0));
}

Uart::~Uart() {
    this->stop();
}

void Uart::transmit(uint8_t data) const {
    while ((*this->registers_.controlStatusA & _BV(UDRE0)) == 0)
        ;
    *this->registers_.data = data;
}

uint8_t Uart::receive() const {
    while ((*this->registers_.controlStatusA & _BV(RXC0)) == 0)
        ;
    return *this->registers_.data;
}

void Uart::setBaudRate(uint16_t baudRate, bool synchronous) const {
    const uint8_t divisionFactor = synchronous ? 2 : 16;

    *this->registers_.baudRate = (F_CPU / divisionFactor) / baudRate - 1;
}

int Uart::putChar(char data, FILE* stream) {
    if (stream == nullptr || stream->udata == nullptr)
        return -1;

    // We must go through reinterpret_cast because of void *.
    // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
    const Uart* uart = reinterpret_cast<const Uart*>(stream->udata);
    // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)

    uart->transmit(data);

    return 0;
}

int Uart::getChar(FILE* stream) {
    if (stream == nullptr || stream->udata == nullptr)
        return -1;

    // We must go through reinterpret_cast because of void *.
    // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
    Uart* uart = reinterpret_cast<Uart*>(stream->udata);
    // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)

    return uart->receive();
}

FILE* Uart::getEmulatedFile() const {
    return &this->emulatedFile_;
}
