#include "MovementManager.h"

template class MovementManager<uint8_t>;
template class MovementManager<uint16_t>;

template <typename T>
MovementManager<T>::MovementManager() {
    // TODO
}

template <typename T>
MovementManager<T>::~MovementManager() {
    // TODO
}

template <typename T>
void MovementManager<T>::moveForward(float speed) const {
    // TODO
}

template <typename T>
void MovementManager<T>::moveBackward(float speed) const {
    // TODO
}

template <typename T>
void MovementManager<T>::moveLeft(float speed, float curveRatio) const {
    // TODO
}

template <typename T>
void MovementManager<T>::moveRight(float speed, float curveRatio) const {
    // TODO
}

template <typename T>
void MovementManager<T>::stop() const {
    // TODO
}
