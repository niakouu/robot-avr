#include "MovementManager.h"

template class MovementManager<uint8_t, TimerPrescalerSynchronous>;
template class MovementManager<uint16_t, TimerPrescalerSynchronous>;
template class MovementManager<uint8_t, TimerPrescalerAsynchronous>;

template <typename T, typename U>
MovementManager<T, U>::MovementManager(Timer<T, U>& timer,
                                       const Motor<T>&& motorLeft,
                                       const Motor<T>&& motorRight)
    : timer_(timer), motorLeft_(motorLeft), motorRight_(motorRight) {}

template <typename T, typename U>
MovementManager<T, U>::~MovementManager() {
    this->stop();
}

template <typename T, typename U>
void MovementManager<T, U>::moveForward(float speed) {
    this->timer_.setAsPwm(
        {.prescaler = U::Value::CLK_DIV_8,
         .speedA = this->motorLeft_.move(speed, 1.0F, true),
         .speedB = this->motorRight_.move(speed, 1.0F, true),
         .compareOutputModeA = TimerCompareOutputModeA::CLEAR,
         .compareOutputModeB = TimerCompareOutputModeB::CLEAR});
}

template <typename T, typename U>
void MovementManager<T, U>::moveBackward(float speed) {
    this->timer_.setAsPwm(
        {.prescaler = U::Value::CLK_DIV_8,
         .speedA = this->motorLeft_.move(speed, 1.0F, false),
         .speedB = this->motorRight_.move(speed, 1.0F, false),
         .compareOutputModeA = TimerCompareOutputModeA::CLEAR,
         .compareOutputModeB = TimerCompareOutputModeB::CLEAR});
}

template <typename T, typename U>
void MovementManager<T, U>::moveLeft(float speed, float curveRatio) {
    this->timer_.setAsPwm(
        {.prescaler = U::Value::CLK_DIV_8,
         .speedA = this->motorLeft_.move(speed, 1.0F, false),
         .speedB = this->motorRight_.move(speed, curveRatio, false),
         .compareOutputModeA = TimerCompareOutputModeA::CLEAR,
         .compareOutputModeB = TimerCompareOutputModeB::CLEAR});
}

template <typename T, typename U>
void MovementManager<T, U>::moveRight(float speed, float curveRatio) {
    this->timer_.setAsPwm(
        {.prescaler = U::Value::CLK_DIV_8,
         .speedA = this->motorLeft_.move(speed, curveRatio, false),
         .speedB = this->motorRight_.move(speed, 1.0F, false),
         .compareOutputModeA = TimerCompareOutputModeA::CLEAR,
         .compareOutputModeB = TimerCompareOutputModeB::CLEAR});
}

template <typename T, typename U>
void MovementManager<T, U>::stop() {
    this->timer_.setAsPwm(
        {.prescaler = U::Value::CLK_DIV_8,
         .speedA = 0,
         .speedB = 0,
         .compareOutputModeA = TimerCompareOutputModeA::CLEAR,
         .compareOutputModeB = TimerCompareOutputModeB::CLEAR});
}

template <typename T, typename U>
void MovementManager<T, U>::setMotorOffsets(float offsetLeft,
                                            float offsetRight) {
    this->motorLeft_.setOffset(offsetLeft);
    this->motorRight_.setOffset(offsetRight);
}
