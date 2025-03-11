#include "Button.h"

Button::Button(Pin::Region region, Pin::Id id, bool pressedIsHigh)
    : isPressed_(false), eventConsumed_(false), pressedIsHigh_(pressedIsHigh),
      buttonPin_(region, id, Pin::Direction::IN) {}

bool Button::isPressed() {
    if (!this->eventConsumed_) {
        this->consumeEvent();
        return false;
    }

    this->restoreEvent();
    return true;
}

bool Button::isEvent() const {
    return this->eventConsumed_;
}

void Button::consumeEvent() {
    this->eventConsumed_ = true;
}

void Button::restoreEvent() {
    this->eventConsumed_ = false;
}

void Button::setPressed() {
    if (this->buttonPin_.read())
        this->isPressed_ = isPressed();
}
