#include "Timer.h"

template class Timer<uint8_t>;
template class Timer<uint16_t>;

template <class T>
Timer<T>::Timer(Timer<T>::Registers&& Registers) : register{
    // TODO
}

template <typename T>
Timer<T>::~Timer() {
    // TODO
}

template <typename T>
void Timer<T>::updateConfig(const ConfigCounter& config) {
    // TODO
}

template <typename T>
void Timer<T>::updateConfig(const ConfigPwm& config) {
    // TODO
}

template <typename T>
void Timer<T>::start() {
    // TODO
}

template <typename T>
void Timer<T>::stop() {
    // TODO
}
