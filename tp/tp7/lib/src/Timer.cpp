#include "Timer.h"

template class Timer<uint8_t, TimerPrescaler>;
template class Timer<uint16_t, TimerPrescaler>;

const Timer<uint8_t, TimerPrescalerSynchronous>::Registers TIMER0_REGISTERS{
    .waveformA{Pin::Region::B, Pin::Id::P3},
    .waveformB{Pin::Region::B, Pin::Id::P4},
    .counter = &TCNT0,
    .compareA = &OCR0A,
    .compareB = &OCR0B,
    .controlA = &TCCR0A,
    .controlB = &TCCR0B,
    .controlC = nullptr,
    .interruptMask = &TIMSK0};

const Timer<uint16_t, TimerPrescalerSynchronous>::Registers TIMER1_REGISTERS{
    .waveformA{Pin::Region::D, Pin::Id::P5},
    .waveformB{Pin::Region::D, Pin::Id::P4},
    .counter = &TCNT1,
    .compareA = &OCR1A,
    .compareB = &OCR1B,
    .controlA = &TCCR1A,
    .controlB = &TCCR1B,
    .controlC = &TCCR1C,
    .interruptMask = &TIMSK1};

const Timer<uint8_t, TimerPrescalerAsynchronous>::Registers TIMER2_REGISTERS{
    .waveformA{Pin::Region::D, Pin::Id::P7},
    .waveformB{Pin::Region::D, Pin::Id::P6},
    .counter = &TCNT2,
    .compareA = &OCR2A,
    .compareB = &OCR2B,
    .controlA = &TCCR2A,
    .controlB = &TCCR2B,
    .controlC = nullptr,
    .interruptMask = &TIMSK2};

template <typename T, typename U>
Timer<T, U>::Timer(const Timer<T, U>::Registers& registers)
    : registers_(registers) {}

template <typename T, typename U>
Timer<T, U>::~Timer() {
    // TODO
}

template <typename T, typename U>
void Timer<T, U>::setAsCounter(T maxTicks, const U& genericPrescaler) {
    const TimerPrescaler& prescaler =
        static_cast<const TimerPrescaler&>(genericPrescaler);
    
    (void)prescaler;
}

template <typename T, typename U>
void Timer<T, U>::setAsCounterFromMilliseconds(uint16_t milliseconds,
                                               const U& prescaler) {}

template <typename T, typename U>
void Timer<T, U>::setAsPwm(T max, float ratioA, float ratioB,
                           const U& prescaler) {}

template <typename T, typename U>
void Timer<T, U>::start() {
    // TODO
}

template <typename T, typename U>
void Timer<T, U>::stop() {
    // TODO
}

constexpr TimerPrescalerSynchronous::TimerPrescalerSynchronous(Value value)
    : value_(value) {}

constexpr TimerPrescalerSynchronous::operator Value() const {
    return this->value_;
}

uint8_t TimerPrescalerSynchronous::getFlags() const {
    return static_cast<uint8_t>(this->value_);
}

uint16_t TimerPrescalerSynchronous::getDivisionFactor() const {
    switch (this->value_) {
        default:
        case Value::CLK_NONE_DIV:
            return 0;
        case Value::CLK_DIV_8:
            return 8;
        case Value::CLK_DIV_64:
            return 64;
        case Value::CLK_DIV_256:
            return 256;
        case Value::CLK_DIV_1024:
            return 1024;
    }
}

constexpr TimerPrescalerAsynchronous::TimerPrescalerAsynchronous(Value value)
    : value_(value) {}

constexpr TimerPrescalerAsynchronous::operator Value() const {
    return this->value_;
}

uint8_t TimerPrescalerAsynchronous::getFlags() const {
    return static_cast<uint8_t>(this->value_);
}

uint16_t TimerPrescalerAsynchronous::getDivisionFactor() const {
    switch (this->value_) {
        default:
        case Value::CLK_NONE_DIV:
            return 0;
        case Value::CLK_DIV_8:
            return 8;
        case Value::CLK_DIV_32:
            return 32;
        case Value::CLK_DIV_64:
            return 64;
        case Value::CLK_DIV_128:
            return 128;
        case Value::CLK_DIV_256:
            return 256;
        case Value::CLK_DIV_1024:
            return 1024;
    }
}
