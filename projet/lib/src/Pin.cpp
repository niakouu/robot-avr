#include "Pin.h"

#include <avr/io.h>

Pin::Pin(Region region, Id id) noexcept
    : mappings_(getMappings(region, id)), registers_(getRegisters(region)) {}

Pin::Pin(Region region, Id id, Direction direction) : Pin(region, id) {
    this->updateDirection(direction);
}

bool Pin::read() const volatile {
    return (*this->registers_.pin & this->mappings_.pinBit) != 0;
}

void Pin::write(bool set) const volatile {
    if (set)
        this->set();
    else
        this->unset();
}

void Pin::set() const volatile {
    *this->registers_.port |= this->mappings_.portBit;
}

void Pin::unset() const volatile {
    *this->registers_.port &= ~this->mappings_.portBit;
}

void Pin::updateDirection(Direction direction) const {
    if (direction == Direction::IN)
        *this->registers_.dataDirection &= ~this->mappings_.directionBit;
    else
        *this->registers_.dataDirection |= this->mappings_.directionBit;
}

#define MAPPING_DEF(region, idx)                                               \
    [static_cast<uint8_t>(                                                     \
        Pin::Id::P##idx)] = {.pinBit = _BV(PIN##region##idx),                  \
                             .portBit = _BV(PORT##region##idx),                \
                             .directionBit = _BV(DD##region##idx)}

#define REGION_DEF(region)                                                     \
    [static_cast<uint8_t>(Pin::Region::region)] = {MAPPING_DEF(region, 0),     \
                                                   MAPPING_DEF(region, 1),     \
                                                   MAPPING_DEF(region, 2),     \
                                                   MAPPING_DEF(region, 3),     \
                                                   MAPPING_DEF(region, 4),     \
                                                   MAPPING_DEF(region, 5),     \
                                                   MAPPING_DEF(region, 6),     \
                                                   MAPPING_DEF(region, 7)}

namespace {
    constexpr Pin::Mappings MAPPINGS[][Pin::NUMBER_OF_PINS] = {REGION_DEF(A),
                                                               REGION_DEF(B),
                                                               REGION_DEF(C),
                                                               REGION_DEF(D)};
} // namespace

const constexpr Pin::Mappings& Pin::getMappings(Region region, Id id) {
    const uint8_t regionIndex = static_cast<uint8_t>(region);
    const uint8_t idIndex = static_cast<uint8_t>(id);

    return ::MAPPINGS[regionIndex][idIndex];
}

#undef REGION_DEF
#undef MAPPING_DEF

constexpr Pin::Registers Pin::getRegisters(Region region) {
    Registers registers{};
    switch (region) {
        case Region::A:
            registers.dataDirection = &DDRA;
            registers.pin = &PINA;
            registers.port = &PORTA;
            break;
        case Region::B:
            registers.dataDirection = &DDRB;
            registers.pin = &PINB;
            registers.port = &PORTB;
            break;
        case Region::C:
            registers.dataDirection = &DDRC;
            registers.pin = &PINC;
            registers.port = &PORTC;
            break;
        case Region::D:
            registers.dataDirection = &DDRD;
            registers.pin = &PIND;
            registers.port = &PORTD;
            break;
        default:
            break;
    }

    return registers;
}
