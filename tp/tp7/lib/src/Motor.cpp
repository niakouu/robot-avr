#include "Motor.h"
#include "Timer.h"

Motor::Motor(const Pin&& directionPin, float offset)
    : directionPin_(directionPin), offset_(offset) {}

Motor::~Motor() {
    this->directionPin_.unset();
}

float Motor::move(float speedRatio, bool forward) const {
    if (forward)
        this->directionPin_.unset();
    else
        this->directionPin_.set();

    return (TimerConstants::TOP_PWM * speedRatio) + this->offset_;
}

void Motor::setOffset(float offset) {
    this->offset_ = offset;
}
