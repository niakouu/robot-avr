#include "Pin.h"

#include <avr/io.h>

Pin::Pin(Region region, Direction direction, Id id)
    : mappings_(getMappings(region, id)) {
    switch (region) {
        case Region::A:
            this->updateDirection(&DDRA, direction);
            if (direction == Direction::IN)
                this->pinPort_ = &PINA;
            else
                this->pinPort_ = &PORTA;
            break;
        case Region::B:
            this->updateDirection(&DDRB, direction);
            if (direction == Direction::IN)
                this->pinPort_ = &PINB;
            else
                this->pinPort_ = &PORTB;
            break;
        case Region::C:
            this->updateDirection(&DDRC, direction);
            if (direction == Direction::IN)
                this->pinPort_ = &PINC;
            else
                this->pinPort_ = &PORTC;
            break;
        case Region::D:
            this->updateDirection(&DDRD, direction);
            if (direction == Direction::IN)
                this->pinPort_ = &PIND;
            else
                this->pinPort_ = &PORTD;
            break;
        default:
            break;
    }
}

Pin::~Pin() {}

bool Pin::read() const {
    return (*this->pinPort_ & this->mappings_->pinBit) != 0;
}

void Pin::write(bool set) const {
    if (set)
        this->set();
    else
        this->unset();
}

void Pin::set() const {
    *this->pinPort_ |= this->mappings_->portBit;
}

void Pin::unset() const {
    *this->pinPort_ &= ~this->mappings_->portBit;
}

void Pin::updateDirection(volatile uint8_t* dataDirectionRegister,
                          const Direction& direction) {
    if (direction == Direction::IN)
        *dataDirectionRegister &= ~this->mappings_->directionBit;
    else
        *dataDirectionRegister |= this->mappings_->directionBit;
}

#define _MAPPING_DEF(region, idx)                                              \
    [static_cast<uint8_t>(Pin::Id::P##idx)] = {_BV(DD##region##idx),           \
                                               _BV(PIN##region##idx),          \
                                               _BV(PORT##region##idx)}

#define _REGION_DEF(region)                                                    \
    [static_cast<uint8_t>(Pin::Region::region)] = {                            \
        _MAPPING_DEF(region, 0), _MAPPING_DEF(region, 1),                      \
        _MAPPING_DEF(region, 2), _MAPPING_DEF(region, 3),                      \
        _MAPPING_DEF(region, 4), _MAPPING_DEF(region, 5),                      \
        _MAPPING_DEF(region, 6), _MAPPING_DEF(region, 7),                      \
    }

constexpr Pin::Mappings mappings[][8] = {_REGION_DEF(A), _REGION_DEF(B),
                                         _REGION_DEF(C), _REGION_DEF(D)};

const Pin::Mappings* Pin::getMappings(const Region& region, const Id& id) {
    return &mappings[static_cast<uint8_t>(region)][static_cast<uint8_t>(id)];
}
