#include "Button.h"

#include <avr/io.h>
#include <util/delay.h>

namespace {
    constexpr uint8_t DEBOUNCE_MS = 30;

    constexpr Pin::Id pinIdForInterrupt(Button::Interrupt interrupt) {
        switch (interrupt) {
            case Button::Interrupt::I0:
                return Pin::Id::P2;
            case Button::Interrupt::I1:
                return Pin::Id::P3;
            case Button::Interrupt::I2:
                return Pin::Id::P2;
        }

        return Pin::Id::P0;
    }
} // namespace

Button::Button(Pin::Region region, Pin::Id id, bool pressedIsHigh)
    : pressedIsHigh_(pressedIsHigh), isPressed_(false), eventConsumed_(false),
      buttonPin_(region, id, Pin::Direction::IN),
      interrupts_{.enabled_ = false} {}

Button::Button(Interrupt interrupt, bool pressedIsHigh)
    : Button(interrupt == Interrupt::I2 ? Pin::Region::B : Pin::Region::D,
             pinIdForInterrupt(interrupt), pressedIsHigh) {
    this->interrupts_.enabled_ = true;
    switch (interrupt) {
        case Button::Interrupt::I0:
            this->interrupts_.mappings_.interruptMask = _BV(INT0);
            this->interrupts_.mappings_.senseControl[0] = _BV(ISC00);
            this->interrupts_.mappings_.senseControl[1] = _BV(ISC01);
            break;
        case Button::Interrupt::I1:
            this->interrupts_.mappings_.interruptMask = _BV(INT1);
            this->interrupts_.mappings_.senseControl[0] = _BV(ISC10);
            this->interrupts_.mappings_.senseControl[1] = _BV(ISC11);
            break;
        case Button::Interrupt::I2:
            this->interrupts_.mappings_.interruptMask = _BV(INT2);
            this->interrupts_.mappings_.senseControl[0] = _BV(ISC20);
            this->interrupts_.mappings_.senseControl[1] = _BV(ISC21);
            break;
        default:
            this->interrupts_.enabled_ = false;
    }

    if (this->interrupts_.enabled_) {
        EIMSK |= this->interrupts_.mappings_.interruptMask;

        // ISR called on every edge
        EICRA |= this->interrupts_.mappings_.senseControl[0];
        EICRA &= ~this->interrupts_.mappings_.senseControl[1];
    }
}

Button::~Button() {
    if (this->interrupts_.enabled_) {
        EIMSK &= ~this->interrupts_.mappings_.interruptMask;
        EICRA &= ~(this->interrupts_.mappings_.senseControl[1]
                   | this->interrupts_.mappings_.senseControl[0]);
    }
}

bool Button::isPressed() const volatile {
    return this->isPressed_;
}

bool Button::isEvent() const volatile {
    return !this->eventConsumed_;
}

void Button::consumeEvent() volatile {
    this->eventConsumed_ = true;
}

void Button::restoreEvent() volatile {
    this->eventConsumed_ = false;
}

void Button::setPressed() volatile {
    _delay_ms(DEBOUNCE_MS);

    const bool oldValue = this->isPressed_;

    this->isPressed_ = this->buttonPin_.read();

    if (oldValue != this->isPressed_) {
        this->restoreEvent();
    }
}
