//
// Travail : tp7
// Section # : 4
// Équipe # : 8290
// Correcteur : Dorine Dantrun
// ----------------------------------------------------------
// Développeurs : Edelina Alieva, Kyle Bouchard, Ilyes Belhaddad et Mohamed
// Elyes Bradai
//
// Description du Timer.h :
//        Implémentation générique des Timers sur le Atmega324PA. Les modes
//        possibles sont le mode CTC et le mode PWM pour tout les types de
//        timer. Le PWM de fréquence est seulement possible avec le Timer 1.
//

#ifndef _TIMER_H
#define _TIMER_H

#include <avr/io.h>
#include <stdio.h>

#include "Pin.h"

enum class TimerCompareOutputModeA : uint8_t {
    NORMAL = 0,
    TOGGLE = _BV(COM0A0),
    CLEAR = _BV(COM0A1),
    SET = _BV(COM0A1) | _BV(COM0A0)
};

enum class TimerCompareOutputModeB : uint8_t {
    NORMAL = 0,
    TOGGLE = _BV(COM0B0),
    CLEAR = _BV(COM0B1),
    SET = _BV(COM0B1) | _BV(COM0B0)
};

template <typename T, typename U>
class Timer {
public:
    struct Registers {
        Pin waveformA, waveformB;
        volatile T *counter, *compareA, *compareB;
        volatile uint8_t *controlA, *controlB, *controlC, *interruptMask;
    };

    struct ConfigCounter {
        T maxTicks;
        U prescaler;
        TimerCompareOutputModeA compareOutputMode;
        static ConfigCounter
        fromMilliseconds(uint16_t milliseconds,
                         TimerCompareOutputModeA compareOutputMode);
    };

    struct ConfigPwm {
        U prescaler;
        T speedA, speedB;
        TimerCompareOutputModeA compareOutputModeA;
        TimerCompareOutputModeB compareOutputModeB;
    };

    Timer(Timer&) = delete;
    void operator=(const Timer&) = delete;

    // CTC mode with output compare match
    void setAsCounter(const ConfigCounter& configCounter);

    // PWM, Phase Correct 8-bit
    void setAsPwm(const ConfigPwm& configPwm);

    void start();
    void stop() const;

    bool isCounterExpired() const;
    void setCounterExpired();

protected:
    friend class Board;
    Timer(const Registers& registers);
    ~Timer();

private:
    friend class Timer1;
    static const uint16_t MILLIS_IN_SECONDS = 1000U;
    const Registers& registers_;
    uint8_t prescalerFlags_, compareOutputModeFlags_;
    volatile bool counterExpired_;
};

class TimerPrescaler {
public:
    enum class PrescaleFactor : uint16_t {
        FACTOR_NONE = 1U,
        FACTOR_8 = 8U,
        FACTOR_32 = 32U,
        FACTOR_64 = 64U,
        FACTOR_128 = 128U,
        FACTOR_256 = 256U,
        FACTOR_1024 = 1024U
    };

    virtual uint8_t getFlags() const = 0;
    virtual PrescaleFactor getDivisionFactor() const = 0;
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

    TimerPrescalerSynchronous(Value value);
    operator Value() const;

    uint8_t getFlags() const;
    TimerPrescaler::PrescaleFactor getDivisionFactor() const;
    static TimerPrescalerSynchronous
    prescalerForDuration(uint16_t milliseconds);

private:
    Value value_;
};

class Timer1 : public Timer<uint16_t, TimerPrescalerSynchronous> {
public:
    Timer1(Timer1&) = delete;
    void operator=(const Timer1&) = delete;

    struct ConfigFrequency {
        uint16_t top;
        TimerPrescalerSynchronous prescaler;
        TimerCompareOutputModeA compareOutputModeA;
        TimerCompareOutputModeB compareOutputModeB;

        static ConfigFrequency
        fromFrequency(float frequency,
                      TimerCompareOutputModeA compareOutputModeA,
                      TimerCompareOutputModeB compareOutputModeB);
    };

    // PWM Phase and Frequency Correct -> mode 8
    void setAsPwmFrequency(const ConfigFrequency& configFrequency);

protected:
    friend class Board;
    Timer1(const Registers& registers);
    ~Timer1();
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

    TimerPrescalerAsynchronous(Value value);
    operator Value() const;

    uint8_t getFlags() const;
    TimerPrescaler::PrescaleFactor getDivisionFactor() const;
    static TimerPrescalerAsynchronous
    prescalerForDuration(uint16_t milliseconds);

private:
    const Value value_;
};

namespace TimerConstants {
    const uint8_t TOP_PWM = 0xFFU;

    extern const Timer<uint8_t, TimerPrescalerSynchronous>::Registers
        TIMER0_REGISTERS;
    extern const Timer<uint16_t, TimerPrescalerSynchronous>::Registers
        TIMER1_REGISTERS;
    extern const Timer<uint8_t, TimerPrescalerAsynchronous>::Registers
        TIMER2_REGISTERS;
}; // namespace TimerConstants

typedef Timer<uint8_t, TimerPrescalerSynchronous> Timer0;
typedef Timer<uint8_t, TimerPrescalerAsynchronous> Timer2;

#endif /* _TIMER_H */
