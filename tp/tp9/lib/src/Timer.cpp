#include "Timer.h"

#include <math.h>
#include <util/atomic.h>

#include "common.h"

ASSERT_REGISTER_FLAGS_MATCH(CS00, CS10);
ASSERT_REGISTER_FLAGS_MATCH(CS01, CS11);
ASSERT_REGISTER_FLAGS_MATCH(CS02, CS12);
ASSERT_REGISTER_FLAGS_MATCH(COM0A1, COM1A1, COM2A1);
ASSERT_REGISTER_FLAGS_MATCH(COM0A0, COM1A0, COM2A0);
ASSERT_REGISTER_FLAGS_MATCH(COM0B1, COM1B1, COM2B1);
ASSERT_REGISTER_FLAGS_MATCH(COM0B0, COM1B0, COM2B0);
ASSERT_REGISTER_FLAGS_MATCH(OCIE0A, OCIE1A, OCIE2A);
ASSERT_REGISTER_FLAGS_MATCH(WGM02, WGM12, WGM22);
ASSERT_REGISTER_FLAGS_MATCH(WGM01, WGM11, WGM21);
ASSERT_REGISTER_FLAGS_MATCH(WGM00, WGM10, WGM20);

template class Timer<uint8_t, TimerPrescalerSynchronous>;
template class Timer<uint16_t, TimerPrescalerSynchronous>;
template class Timer<uint8_t, TimerPrescalerAsynchronous>;

const Timer<uint8_t, TimerPrescalerSynchronous>::Registers
    TimerConstants::TIMER0_REGISTERS{.waveformA{Pin::Region::B, Pin::Id::P3},
                                     .waveformB{Pin::Region::B, Pin::Id::P4},
                                     .counter = &TCNT0,
                                     .compareA = &OCR0A,
                                     .compareB = &OCR0B,
                                     .controlA = &TCCR0A,
                                     .controlB = &TCCR0B,
                                     .controlC = nullptr,
                                     .interruptMask = &TIMSK0};

const Timer<uint16_t, TimerPrescalerSynchronous>::Registers
    TimerConstants::TIMER1_REGISTERS{.waveformA{Pin::Region::D, Pin::Id::P5},
                                     .waveformB{Pin::Region::D, Pin::Id::P4},
                                     .counter = &TCNT1,
                                     .compareA = &OCR1A,
                                     .compareB = &OCR1B,
                                     .controlA = &TCCR1A,
                                     .controlB = &TCCR1B,
                                     .controlC = &TCCR1C,
                                     .interruptMask = &TIMSK1};

const Timer<uint8_t, TimerPrescalerAsynchronous>::Registers
    TimerConstants::TIMER2_REGISTERS{.waveformA{Pin::Region::D, Pin::Id::P7},
                                     .waveformB{Pin::Region::D, Pin::Id::P6},
                                     .counter = &TCNT2,
                                     .compareA = &OCR2A,
                                     .compareB = &OCR2B,
                                     .controlA = &TCCR2A,
                                     .controlB = &TCCR2B,
                                     .controlC = nullptr,
                                     .interruptMask = &TIMSK2};

namespace {
    template <TimerPrescaler::PrescaleFactor prescaleFactor>
    constexpr uint32_t maxFrequency =
        (F_CPU / 2) / static_cast<uint16_t>(prescaleFactor);

    template <TimerPrescaler::PrescaleFactor prescaleFactor>
    constexpr uint16_t prescaleMaximumMilliseconds =
        (UINT16_MAX * 1000) / (F_CPU / static_cast<uint16_t>(prescaleFactor));
} // namespace

template <typename T, typename U>
Timer<T, U>::Timer(const Timer<T, U>::Registers& registers)
    : registers_(registers), prescalerFlags_(0), counterExpired_(false) {
    this->registers_.waveformA.updateDirection(Pin::Direction::OUT);
    this->registers_.waveformB.updateDirection(Pin::Direction::OUT);
}

template <typename T, typename U>
Timer<T, U>::~Timer() {
    *this->registers_.counter = 0;
    *this->registers_.compareA = 0;
    *this->registers_.compareB = 0;
    *this->registers_.controlA = 0;
    *this->registers_.controlB = 0;
    if (this->registers_.controlC != nullptr)
        *this->registers_.controlC = 0;
    *this->registers_.interruptMask = 0;
}

template <typename T, typename U>
void Timer<T, U>::setAsCounter(const ConfigCounter& configCounter) {
    const TimerPrescaler& prescaler =
        static_cast<const TimerPrescaler&>(configCounter.prescaler);
    this->prescalerFlags_ = prescaler.getFlags();
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        this->stop();

        *this->registers_.counter = 0;
        *this->registers_.compareA = configCounter.maxTicks;
        *this->registers_.compareB = 0;
        *this->registers_.controlA =
            static_cast<uint8_t>(configCounter.compareOutputMode)
            | (*this->registers_.controlA & ~(_BV(COM0B1) | _BV(COM0B0)));
        *this->registers_.controlB = 0;
        if (this->registers_.controlC != nullptr)
            *this->registers_.controlC = 0;
        *this->registers_.interruptMask = _BV(OCIE0A);

        if (isType<T, uint16_t>::value) {
            *this->registers_.controlA &= ~(_BV(WGM11) | _BV(WGM10));
            *this->registers_.controlB =
                _BV(WGM12) | (*this->registers_.controlB & ~_BV(WGM13));
        } else {
            *this->registers_.controlA =
                _BV(WGM01) | (*this->registers_.controlA & ~(_BV(WGM00)));
        }
    }
}

template <typename T, typename U>
void Timer<T, U>::setAsPwm(const ConfigPwm& configPwm) {
    const TimerPrescaler& prescaler =
        static_cast<const TimerPrescaler&>(configPwm.prescaler);
    this->prescalerFlags_ = prescaler.getFlags();

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        this->stop();

        *this->registers_.counter = 0;
        *this->registers_.compareA = configPwm.speedA;
        *this->registers_.compareB = configPwm.speedB;

        *this->registers_.controlA =
            static_cast<uint8_t>(configPwm.compareOutputModeA)
            | static_cast<uint8_t>(configPwm.compareOutputModeB) | _BV(WGM00);

        *this->registers_.controlB = *this->registers_.controlB & ~(_BV(WGM02));

        if (isType<T, uint16_t>::value)
            *this->registers_.controlB &= ~(_BV(WGM13));

        if (this->registers_.controlC != nullptr)
            *this->registers_.controlC = 0;

        *this->registers_.interruptMask = 0;
    }
}

template <typename T, typename U>
typename Timer<T, U>::ConfigCounter
Timer<T, U>::ConfigCounter::fromMilliseconds(
    uint16_t milliseconds, TimerCompareOutputModeA compareOutputMode) {

    U prescaler = U::prescalerForDuration(milliseconds);

    uint32_t maxTicks = static_cast<uint32_t>(
        (static_cast<float>(F_CPU)
         / static_cast<uint16_t>(prescaler.getDivisionFactor()))
        * static_cast<float>(milliseconds) / MILLIS_IN_SECONDS);

    if (isType<T, uint8_t>::value && maxTicks > UINT8_MAX)
        maxTicks = UINT8_MAX;
    else if (isType<T, uint16_t>::value && maxTicks > UINT16_MAX)
        maxTicks = UINT16_MAX;

    return ConfigCounter{.maxTicks = static_cast<T>(maxTicks),
                         .prescaler = prescaler,
                         .compareOutputMode = compareOutputMode};
}

template <typename T, typename U>
void Timer<T, U>::start() {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        this->counterExpired_ = false;
        *this->registers_.controlB |= this->prescalerFlags_;
    }
}

template <typename T, typename U>
void Timer<T, U>::stop() const {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        *this->registers_.controlB &= ~(_BV(CS12) | _BV(CS11) | _BV(CS10));
    }
}

template <typename T, typename U>
bool Timer<T, U>::isCounterExpired() const {
    return this->counterExpired_;
}

template <typename T, typename U>
void Timer<T, U>::setCounterExpired() {
    this->counterExpired_ = true;
}

TimerPrescalerSynchronous::TimerPrescalerSynchronous(Value value)
    : value_(value) {}

TimerPrescalerSynchronous::operator Value() const {
    return this->value_;
}

uint8_t TimerPrescalerSynchronous::getFlags() const {
    return static_cast<uint8_t>(this->value_);
}

TimerPrescaler::PrescaleFactor
TimerPrescalerSynchronous::getDivisionFactor() const {
    switch (this->value_) {
        default:
        case Value::CLK_NONE_DIV:
            return PrescaleFactor::FACTOR_NONE;
        case Value::CLK_DIV_8:
            return PrescaleFactor::FACTOR_8;
        case Value::CLK_DIV_64:
            return PrescaleFactor::FACTOR_64;
        case Value::CLK_DIV_256:
            return PrescaleFactor::FACTOR_256;
        case Value::CLK_DIV_1024:
            return PrescaleFactor::FACTOR_1024;
    }
}

TimerPrescalerSynchronous
TimerPrescalerSynchronous::prescalerForDuration(uint16_t milliseconds) {
    if (milliseconds
        <= ::prescaleMaximumMilliseconds<PrescaleFactor::FACTOR_NONE>)
        return Value::CLK_NONE_DIV;

    if (milliseconds <= ::prescaleMaximumMilliseconds<PrescaleFactor::FACTOR_8>)
        return Value::CLK_DIV_8;

    if (milliseconds
        <= ::prescaleMaximumMilliseconds<PrescaleFactor::FACTOR_64>)
        return Value::CLK_DIV_64;

    if (milliseconds
        <= ::prescaleMaximumMilliseconds<PrescaleFactor::FACTOR_256>)
        return Value::CLK_DIV_256;

    return Value::CLK_DIV_1024;
}

TimerPrescalerAsynchronous::TimerPrescalerAsynchronous(Value value)
    : value_(value) {}

TimerPrescalerAsynchronous::operator Value() const {
    return this->value_;
}

uint8_t TimerPrescalerAsynchronous::getFlags() const {
    return static_cast<uint8_t>(this->value_);
}

TimerPrescaler::PrescaleFactor
TimerPrescalerAsynchronous::getDivisionFactor() const {
    switch (this->value_) {
        default:
        case Value::CLK_NONE_DIV:
            return PrescaleFactor::FACTOR_NONE;
        case Value::CLK_DIV_8:
            return PrescaleFactor::FACTOR_8;
        case Value::CLK_DIV_32:
            return PrescaleFactor::FACTOR_32;
        case Value::CLK_DIV_64:
            return PrescaleFactor::FACTOR_64;
        case Value::CLK_DIV_128:
            return PrescaleFactor::FACTOR_128;
        case Value::CLK_DIV_256:
            return PrescaleFactor::FACTOR_256;
        case Value::CLK_DIV_1024:
            return PrescaleFactor::FACTOR_1024;
    }
}

TimerPrescalerAsynchronous
TimerPrescalerAsynchronous::prescalerForDuration(uint16_t milliseconds) {
    if (milliseconds
        <= ::prescaleMaximumMilliseconds<PrescaleFactor::FACTOR_NONE>)
        return Value::CLK_NONE_DIV;

    if (milliseconds <= ::prescaleMaximumMilliseconds<PrescaleFactor::FACTOR_8>)
        return Value::CLK_DIV_8;

    if (milliseconds
        <= ::prescaleMaximumMilliseconds<PrescaleFactor::FACTOR_32>)
        return Value::CLK_DIV_32;

    if (milliseconds
        <= ::prescaleMaximumMilliseconds<PrescaleFactor::FACTOR_64>)
        return Value::CLK_DIV_64;

    if (milliseconds
        <= ::prescaleMaximumMilliseconds<PrescaleFactor::FACTOR_128>)
        return Value::CLK_DIV_128;

    if (milliseconds
        <= ::prescaleMaximumMilliseconds<PrescaleFactor::FACTOR_256>)
        return Value::CLK_DIV_256;

    return Value::CLK_DIV_1024;
}

Timer1::Timer1(const Registers& registers) : Timer(registers) {}

Timer1::~Timer1() {
    Timer::~Timer();
}

Timer1::ConfigFrequency Timer1::ConfigFrequency::fromFrequency(
    uint32_t frequency, TimerCompareOutputModeA compareOutputModeA,
    TimerCompareOutputModeB compareOutputModeB) {

    TimerPrescalerSynchronous::Value prescaleFactor{
        TimerPrescalerSynchronous::Value::CLK_NONE_DIV};

    if (frequency
        <= ::maxFrequency<TimerPrescaler::PrescaleFactor::FACTOR_1024>)
        prescaleFactor = TimerPrescalerSynchronous::Value::CLK_DIV_1024;
    else if (frequency
             <= ::maxFrequency<TimerPrescaler::PrescaleFactor::FACTOR_256>)
        prescaleFactor = TimerPrescalerSynchronous::Value::CLK_DIV_256;
    else if (frequency
             <= ::maxFrequency<TimerPrescaler::PrescaleFactor::FACTOR_64>)
        prescaleFactor = TimerPrescalerSynchronous::Value::CLK_DIV_64;
    else if (frequency
             <= ::maxFrequency<TimerPrescaler::PrescaleFactor::FACTOR_8>)
        prescaleFactor = TimerPrescalerSynchronous::Value::CLK_DIV_8;
    else if (frequency
             > ::maxFrequency<TimerPrescaler::PrescaleFactor::FACTOR_NONE>)
        frequency = ::maxFrequency<TimerPrescaler::PrescaleFactor::FACTOR_NONE>;

    const uint32_t numerator =
        (F_CPU / 2) / static_cast<uint16_t>(prescaleFactor);

    const uint16_t top = static_cast<uint16_t>(
        roundf(static_cast<float>(numerator) / static_cast<float>(frequency)));

    return {top, prescaleFactor, compareOutputModeA, compareOutputModeB};
}

void Timer1::setAsPwmFrequency(const ConfigFrequency& configFrequency) {
    this->prescalerFlags_ = configFrequency.prescaler.getFlags();

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        this->stop();

        *this->registers_.counter = 0;
        *this->registers_.compareA = configFrequency.top;

        *this->registers_.controlA =
            static_cast<uint8_t>(configFrequency.compareOutputModeA)
            | static_cast<uint8_t>(configFrequency.compareOutputModeB)
            | _BV(WGM10);

        *this->registers_.controlB |= _BV(WGM13);
        *this->registers_.controlB &= ~_BV(WGM12);

        *this->registers_.controlC = 0;
        *this->registers_.interruptMask = 0;
    }
}
