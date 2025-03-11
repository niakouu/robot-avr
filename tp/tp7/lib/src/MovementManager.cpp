#include "MovementManager.h"

template class MovementManager<uint8_t, TimerPrescalerSynchronous>;
template class MovementManager<uint16_t, TimerPrescalerSynchronous>;
template class MovementManager<uint8_t, TimerPrescalerAsynchronous>;

template <typename T, typename U>
MovementManager<T, U>::MovementManager(const Timer<T, U>& timer,
                                       const Motor&& motorLeft,
                                       const Motor&& motorRight)
    : timer_(timer), motorLeft_(motorLeft), motorRight_(motorRight) {}

template <typename T, typename U>
MovementManager<T, U>::~MovementManager() {
    this->stop();
}

template <typename T, typename U>
void MovementManager<T, U>::moveForward(float speed) const {
    this->timer_.setAsPwm(
        {.prescaler = U::Value::CLK_DIV_8,
         .ratioA = static_cast<T>(this->motorLeft_.move(speed, true)),
         .ratioB = static_cast<T>(this->motorRight_.move(speed, true)),
         .compareOutputModeA = TimerCompareOutputModeA::CLEAR,
         .compareOutputModeB = TimerCompareOutputModeB::CLEAR});
}

template <typename T, typename U>
void MovementManager<T, U>::moveBackward(float speed) const {
    this->timer_.setAsPwm(
        {.prescaler = U::Value::CLK_DIV_8,
         .ratioA = static_cast<T>(this->motorLeft_.move(speed, false)),
         .ratioB = static_cast<T>(this->motorRight_.move(speed, false)),
         .compareOutputModeA = TimerCompareOutputModeA::CLEAR,
         .compareOutputModeB = TimerCompareOutputModeB::CLEAR});
}

template <typename T, typename U>
void MovementManager<T, U>::moveLeft(float speed, float curveRatio) const {
    float ratioLeft = this->motorLeft_.move(speed, true);
    this->timer_.setAsPwm(
        {.prescaler = U::Value::CLK_DIV_8,
         .ratioA = static_cast<T>(ratioLeft),
         .ratioB = static_cast<T>(ratioLeft * curveRatio),
         .compareOutputModeA = TimerCompareOutputModeA::CLEAR,
         .compareOutputModeB = TimerCompareOutputModeB::CLEAR});
}

template <typename T, typename U>
void MovementManager<T, U>::moveRight(float speed, float curveRatio) const {
    float ratioRight = this->motorRight_.move(speed, true);
    this->timer_.setAsPwm(
        {.prescaler = U::Value::CLK_DIV_8,
         .ratioA = static_cast<T>(ratioRight * curveRatio),
         .ratioB = static_cast<T>(ratioRight),
         .compareOutputModeA = TimerCompareOutputModeA::CLEAR,
         .compareOutputModeB = TimerCompareOutputModeB::CLEAR});
}

template <typename T, typename U>
void MovementManager<T, U>::stop() const {
    this->timer_.setAsPwm(
        {.prescaler = U::Value::CLK_DIV_8,
         .ratioA = 0,
         .ratioB = 0,
         .compareOutputModeA = TimerCompareOutputModeA::CLEAR,
         .compareOutputModeB = TimerCompareOutputModeB::CLEAR});
}

template <typename T, typename U>
void MovementManager<T, U>::setMotorOffsets(float offsetLeft,
                                            float offsetRight) {
    this->motorLeft_.setOffset(offsetLeft);
    this->motorRight_.setOffset(offsetRight);
}
