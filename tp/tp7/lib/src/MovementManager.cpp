#include "MovementManager.h"

template class MovementManager<uint8_t, TimerPrescalerSynchronous>;
template class MovementManager<uint16_t, TimerPrescalerSynchronous>;
template class MovementManager<uint8_t, TimerPrescalerAsynchronous>;

template <typename T, typename U>
MovementManager<T, U>::MovementManager(const Timer<T, U>& timer,
                                       Motor&& motorLeft, Motor&& motorRight)
    : timer_(timer), motorLeft_(motorLeft), motorRight_(motorRight) {}

template <typename T, typename U>
MovementManager<T, U>::~MovementManager() {
    // TODO
}

template <typename T, typename U>
void MovementManager<T, U>::moveForward(float speed) const {
    // TODO
}

template <typename T, typename U>
void MovementManager<T, U>::moveBackward(float speed) const {
    // TODO
}

template <typename T, typename U>
void MovementManager<T, U>::moveLeft(float speed, float curveRatio) const {
    // TODO
}

template <typename T, typename U>
void MovementManager<T, U>::moveRight(float speed, float curveRatio) const {
    // TODO
}

template <typename T, typename U>
void MovementManager<T, U>::stop() const {
    // TODO
}
