#include "Motor.h"

#include "Timer.h"

template class Motor<uint8_t>;
template class Motor<uint16_t>;

template <typename T>
Motor<T>::Motor(const Pin&& directionPin, float offset)
    : directionPin_(directionPin), offset_(offset) {}

template <typename T>
Motor<T>::~Motor() {
    this->directionPin_.unset();
}

template <typename T>
T Motor<T>::move(float speedRatio, bool forward) const {
    if (forward)
        this->directionPin_.unset();
    else
        this->directionPin_.set();

    T speed = (TimerConstants::TOP_PWM * speedRatio) + this->offset_;
    if (speed >= TimerConstants::TOP_PWM)
        speed = TimerConstants::TOP_PWM - 1;
    return speed;
}

template <typename T>
void Motor<T>::setOffset(float offset) {
    this->offset_ = offset;
}
