#include "Pin.h"

#include <avr/io.h>

Pin::Pin(Region region, Id id) noexcept
    : mappings_(getMappings(region, id)), registers_(getRegisters(region)) {}

Pin::Pin(Region region, Id id, Direction direction) : Pin(region, id) {
    this->updateDirection(direction);
}

Pin::~Pin() = default;

bool Pin::read() const {
    return (*this->registers_.pin & this->mappings_.pinBit) != 0;
}

void Pin::write(bool set) const {
    if (set)
        this->set();
    else
        this->unset();
}

void Pin::set() const {
    *this->registers_.port |= this->mappings_.portBit;
}

void Pin::unset() const {
    *this->registers_.port &= ~this->mappings_.portBit;
}

void Pin::updateDirection(Direction direction) const {
    if (direction == Direction::IN)
        *this->registers_.dataDirection &= ~this->mappings_.directionBit;
    else
        *this->registers_.dataDirection |= this->mappings_.directionBit;
}

#define _MAPPING_DEF(region, idx)                                              \
    [static_cast<uint8_t>(                                                     \
        Pin::Id::P##idx)] = {.pinBit = _BV(PIN##region##idx),                  \
                             .portBit = _BV(PORT##region##idx),                \
                             .directionBit = _BV(DD##region##idx)}

#define _REGION_DEF(region)                                                    \
    [static_cast<uint8_t>(Pin::Region::region)] = {                            \
        _MAPPING_DEF(region, 0), _MAPPING_DEF(region, 1),                      \
        _MAPPING_DEF(region, 2), _MAPPING_DEF(region, 3),                      \
        _MAPPING_DEF(region, 4), _MAPPING_DEF(region, 5),                      \
        _MAPPING_DEF(region, 6), _MAPPING_DEF(region, 7),                      \
    }

static constexpr Pin::Mappings mappings[][Pin::NUMBER_OF_PINS] = {
    _REGION_DEF(A), _REGION_DEF(B), _REGION_DEF(C), _REGION_DEF(D)};

const constexpr Pin::Mappings& Pin::getMappings(Region region, Id id) {
    uint8_t regionIndex = static_cast<uint8_t>(region);
    uint8_t idIndex = static_cast<uint8_t>(id);

    return mappings[regionIndex][idIndex];
}

#undef _REGION_DEF
#undef _MAPPING_DEF

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
