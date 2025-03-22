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
void MovementManager<T, U>::moveForward(float speedRatio) {
    this->timer_.setAsPwm(
        {.prescaler = U::Value::CLK_DIV_8,
         .speedA = this->motorLeft_.move(speedRatio, true),
         .speedB = this->motorRight_.move(speedRatio, true),
         .compareOutputModeA = TimerCompareOutputModeA::CLEAR,
         .compareOutputModeB = TimerCompareOutputModeB::CLEAR});

    this->timer_.start();
}

template <typename T, typename U>
void MovementManager<T, U>::moveBackward(float speedRatio) {
    this->timer_.setAsPwm(
        {.prescaler = U::Value::CLK_DIV_8,
         .speedA = this->motorLeft_.move(speedRatio, false),
         .speedB = this->motorRight_.move(speedRatio, false),
         .compareOutputModeA = TimerCompareOutputModeA::CLEAR,
         .compareOutputModeB = TimerCompareOutputModeB::CLEAR});

    this->timer_.start();
}

template <typename T, typename U>
void MovementManager<T, U>::moveLeft(float speedRatio, float curveRatio) {
    this->timer_.setAsPwm(
        {.prescaler = U::Value::CLK_DIV_8,
         .speedA = this->motorLeft_.move(speedRatio * curveRatio, false),
         .speedB = this->motorRight_.move(speedRatio, true),
         .compareOutputModeA = TimerCompareOutputModeA::CLEAR,
         .compareOutputModeB = TimerCompareOutputModeB::CLEAR});
    this->timer_.start();
}

template <typename T, typename U>
void MovementManager<T, U>::moveRight(float speedRatio, float curveRatio) {
    this->timer_.setAsPwm(
        {.prescaler = U::Value::CLK_DIV_8,
         .speedA = this->motorLeft_.move(speedRatio, true),
         .speedB = this->motorRight_.move(speedRatio * curveRatio, false),
         .compareOutputModeA = TimerCompareOutputModeA::CLEAR,
         .compareOutputModeB = TimerCompareOutputModeB::CLEAR});
    this->timer_.start();
}

template <typename T, typename U>
void MovementManager<T, U>::stop() {
    this->timer_.setAsPwm(
        {.prescaler = U::Value::CLK_DIV_8,
         .speedA = 0,
         .speedB = 0,
         .compareOutputModeA = TimerCompareOutputModeA::CLEAR,
         .compareOutputModeB = TimerCompareOutputModeB::CLEAR});
    this->timer_.start();
}

template <typename T, typename U>
void MovementManager<T, U>::setMotorOffsets(float offsetLeft,
                                            float offsetRight) {
    this->motorLeft_.setOffset(offsetLeft);
    this->motorRight_.setOffset(offsetRight);
}
