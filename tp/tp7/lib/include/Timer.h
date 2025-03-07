#ifndef _TIMER_H
#define _TIMER_H

#include <avr/io.h>
#include <stdio.h>

#include "Pin.h"

template <typename T, typename U>
class Timer {
public:
    struct Registers {
        Pin waveformA, waveformB;
        volatile T *counter, *compareA, *compareB;
        volatile uint8_t *controlA, *controlB, *controlC, *interruptMask;
    };

    Timer(Timer&) = delete;
    void operator=(const Timer&) = delete;

    void setAsCounter(T maxTicks, const U& prescaler);
    void setAsCounterFromMilliseconds(uint16_t milliseconds,
                                      const U& prescaler);
    void setAsPwm(T max, float ratioA, float ratioB, const U& prescaler);

    void start();
    void stop();

private:
    friend class Board;
    Timer(const Registers& registers);
    ~Timer();
    const Registers& registers_;
};

class TimerPrescaler {
public:
    virtual uint8_t getFlags() const = 0;
    virtual uint16_t getDivisionFactor() const = 0;
};

class TimerPrescalerSynchronous : public TimerPrescaler {
public:
    enum class Value : uint8_t {
        CLK_NONE_DIV = _BV(CS00),
        CLK_DIV_8 = _BV(CS01),
        CLK_DIV_64 = _BV(CS01) | _BV(CS00),
        CLK_DIV_256 = _BV(CS02),
        CLK_DIV_1024 = _BV(CS02) | _BV(CS00)
    };

    static_assert(CS00 == CS10, "CS00 must match CS10");
    static_assert(CS01 == CS11, "CS01 must match CS11");
    static_assert(CS02 == CS12, "CS02 must match CS12");

    constexpr TimerPrescalerSynchronous(Value value);
    constexpr operator Value() const;

    uint8_t getFlags() const;
    uint16_t getDivisionFactor() const;

private:
    Value value_;
};

class TimerPrescalerAsynchronous : public TimerPrescaler {
public:
    enum class Value : uint8_t {
        CLK_NONE_DIV = _BV(CS20),
        CLK_DIV_8 = _BV(CS21),
        CLK_DIV_32 = _BV(CS21) | _BV(CS20),
        CLK_DIV_64 = _BV(CS22),
        CLK_DIV_128 = _BV(CS22) | _BV(CS20),
        CLK_DIV_256 = _BV(CS22) | _BV(CS21),
        CLK_DIV_1024 = _BV(CS22) | _BV(CS21) | _BV(CS20)
    };

    constexpr TimerPrescalerAsynchronous(Value value);
    constexpr operator Value() const;

    uint8_t getFlags() const;
    uint16_t getDivisionFactor() const;

private:
    const Value value_;
};

extern const Timer<uint8_t, TimerPrescalerSynchronous>::Registers
    TIMER0_REGISTERS;
extern const Timer<uint16_t, TimerPrescalerSynchronous>::Registers
    TIMER1_REGISTERS;
extern const Timer<uint8_t, TimerPrescalerAsynchronous>::Registers
    TIMER2_REGISTERS;

#endif /* _TIMER_H */