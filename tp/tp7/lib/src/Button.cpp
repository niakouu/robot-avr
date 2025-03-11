#include "Button.h"

Button::Button(Pin::Region region, Pin::Id id, bool pressedIsHigh)
    : pressedIsHigh_(pressedIsHigh), isPressed_(false), eventConsumed_(false),
      buttonPin_(region, id, Pin::Direction::IN) {}

// TODO(user):

Button::~Button() {}

bool Button::isPressed() {

    if (!isEvent()) {
        consumeEvent();
        return false;
    }
    restoreEvent();
    return true;
}

bool Button::isEvent() const {
    return eventConsumed_;
}

void Button::consumeEvent() {
    eventConsumed_ = true;
}

void Button::restoreEvent() {
    eventConsumed_ = false;
}

void Button::setPressed() {
    if (buttonPin_.read())
        isPressed_ = isPressed();
}
