#include "Button.h"

#include <avr/io.h>
#include <util/delay.h>

constexpr uint8_t DEBOUNCE_MS = 30;

Button::Button(Pin::Region region, Pin::Id id, bool pressedIsHigh)
    : isPressed_(false), eventConsumed_(false), pressedIsHigh_(pressedIsHigh),
      buttonPin_(region, id, Pin::Direction::IN) {
    EIMSK |= _BV(INT0);

    // ISR called on every edge
    EICRA &= ~(_BV(ISC01));
    EICRA |= _BV(ISC00);
}

bool Button::isPressed() const {
    return this->isPressed_;
}

bool Button::isEvent() const {
    return !this->eventConsumed_;
}

void Button::consumeEvent() {
    this->eventConsumed_ = true;
}

void Button::restoreEvent() {
    this->eventConsumed_ = false;
}

void Button::setPressed() {
    _delay_ms(DEBOUNCE_MS);

    const bool oldValue = this->isPressed_;

    this->isPressed_ = this->buttonPin_.read();

    if (oldValue != this->isPressed_) {
        this->restoreEvent();
    }
}
