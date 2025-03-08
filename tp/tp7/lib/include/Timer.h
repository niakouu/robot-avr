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

    enum class CompareOutputModeA {
        NORMAL = 0,
        TOGGLE = _BV(COM0A0),
        CLEAR = _BV(COM0A1),
        SET = _BV(COM0A1) | _BV(COM0A0)
    };

    enum class CompareOutputModeB {
        NORMAL = 0,
        TOGGLE = _BV(COM0B0),
        CLEAR = _BV(COM0B1),
        SET = _BV(COM0B1) | _BV(COM0B0)
    };

    struct ConfigCounter {
        T maxTicks;
        U prescaler;
        CompareOutputModeA compareOutputMode;
        static ConfigCounter
        fromMilliseconds(uint16_t milliseconds, U prescaler,
                         CompareOutputModeA compareOutputMode);
    };

    struct ConfigPwm {
        U prescaler;
        float ratioA, ratioB;
        CompareOutputModeA compareOutputModeA;
        CompareOutputModeB compareOutputModeB;
    };

    static_assert(COM0A0 == COM1A0 && COM0A0 == COM2A0,
                  "COM0A0 must match COM1A0 and COM2A0.");
    static_assert(COM0A1 == COM1A1 && COM0A1 == COM2A1,
                  "COM0A1 must match COM1A1 and COM2A1.");

    Timer(Timer&) = delete;
    void operator=(const Timer&) = delete;

    // CTC mode with output compare match
    void setAsCounter(const ConfigCounter& configCounter);

    // PWM, Phase Correct 8-bit
    void setAsPwm(const ConfigPwm& configPwm);

    void start() const;
    void stop() const;

private:
    friend class Board;
    static const uint16_t MILLIS_IN_SECONDS = 1000;
    const Registers& registers_;
    uint8_t prescalerFlags_;
    Timer(const Registers& registers);
    ~Timer();
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
