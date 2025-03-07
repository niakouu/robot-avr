#include "Motor.h"

Motor::Motor(Pin&& directionPin, float offset) : directionPin_(directionPin) {
    // TODO
}

Motor::~Motor() {
    // TODO : turn off
}

float Motor::move(float speedRatio, bool forward) const {
    // TODO
    return 0.0f;
}
