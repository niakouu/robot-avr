#include "MovementManager.h"
#include "Board.h"

template class MovementManager<uint8_t, TimerPrescalerSynchronous>;
template class MovementManager<uint16_t, TimerPrescalerSynchronous>;
template class MovementManager<uint8_t, TimerPrescalerAsynchronous>;

namespace {
    const constexpr uint16_t KICKSTART_TIME_MS = 250; 
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
    this->kickstartMotors();

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
    this->kickstartMotors();

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
    this->kickstartMotors();

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
    this->kickstartMotors();

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

template <typename T, typename U>
void MovementManager<T, U>::kickstartMotors() {
    constexpr const T MAX_SPEED = ~0;

    this->timer_.setAsPwm(
        {.prescaler = U::Value::CLK_DIV_8,
         .speedA = MAX_SPEED,
         .speedB = MAX_SPEED,
         .compareOutputModeA = TimerCompareOutputModeA::CLEAR,
         .compareOutputModeB = TimerCompareOutputModeB::CLEAR});
    this->timer_.start();

    Board::get().getWatchdogTimer().sleep(KICKSTART_TIME_MS, WatchdogTimer::SleepMode::IDLE);
}