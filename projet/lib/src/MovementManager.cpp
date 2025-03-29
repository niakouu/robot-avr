#include "MovementManager.h"

#include "Board.h"

template class MovementManager<uint8_t, TimerPrescalerSynchronous>;
template class MovementManager<uint16_t, TimerPrescalerSynchronous>;
template class MovementManager<uint8_t, TimerPrescalerAsynchronous>;

namespace {
    const constexpr uint16_t KICKSTART_TIME_MS = 250U;
}

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
    this->kickstartMotors(KickstartDirection::FORWARD,
                          KickstartDirection::FORWARD);

    this->move(true, speedRatio, true, speedRatio);
}

template <typename T, typename U>
void MovementManager<T, U>::moveBackward(float speedRatio) {
    this->kickstartMotors(KickstartDirection::BACKWARD,
                          KickstartDirection::BACKWARD);

    this->move(false, speedRatio, false, speedRatio);
}

template <typename T, typename U>
void MovementManager<T, U>::moveLeft(float speedRatio, float curveRatio) {
    this->kickstartMotors(KickstartDirection::NONE,
                          KickstartDirection::FORWARD);

    this->move(false, speedRatio * curveRatio, true, speedRatio);
}

template <typename T, typename U>
void MovementManager<T, U>::moveRight(float speedRatio, float curveRatio) {
    this->kickstartMotors(KickstartDirection::FORWARD,
                          KickstartDirection::NONE);

    this->move(true, speedRatio, false, speedRatio * curveRatio);
}

template <typename T, typename U>
void MovementManager<T, U>::stop() {
    this->move(false, 0.0F, false, 0.0F);
    this->timer_.stop();
}

template <typename T, typename U>
void MovementManager<T, U>::setMotorOffsets(float offsetLeft,
                                            float offsetRight) {
    this->motorLeft_.setOffset(offsetLeft);
    this->motorRight_.setOffset(offsetRight);
}

template <typename T, typename U>
void MovementManager<T, U>::kickstartMotors(KickstartDirection left,
                                            KickstartDirection right) {
    this->move(left == KickstartDirection::FORWARD,
               left == KickstartDirection::NONE ? 0.0F : 1.0F,
               right == KickstartDirection::FORWARD,
               right == KickstartDirection::NONE ? 0.0F : 1.0F);

    Board::get().getWatchdogTimer().sleep(KICKSTART_TIME_MS,
                                          WatchdogTimer::SleepMode::IDLE);
}

#define DEBUG
#include "debug.h"

template <typename T, typename U>
void MovementManager<T, U>::move(bool leftForward, float leftSpeedRatio,
                                 bool rightForward, float rightSpeedRatio) {
    this->timer_.setAsPwm(
        {.prescaler = U::Value::CLK_DIV_8,
         .speedA = this->motorLeft_.move(leftSpeedRatio, leftForward),
         .speedB = this->motorRight_.move(rightSpeedRatio, rightForward),
         .compareOutputModeA = TimerCompareOutputModeA::CLEAR,
         .compareOutputModeB = TimerCompareOutputModeB::CLEAR});

    this->timer_.start();
}