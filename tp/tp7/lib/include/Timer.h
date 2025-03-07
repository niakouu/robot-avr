#ifndef _TIMER_H
#define _TIMER_H

#include <avr/io.h>
#include <stdio.h>

#include "Pin.h"

template <typename T>
class Timer {
public:
    struct ConfigCounter {
        T max;
        uint8_t prescalar;
    };

    struct ConfigPwm {
        T max;
        float ratios[2];
        uint8_t prescalar;
    };

    struct Registers {
        Pin waveformA, waveformB;
        volatile T *counter, *compareA, *compareB;
        volatile uint8_t *controlA, *controlB, *controlC, *interruptMask;
    };

    Timer(Timer&) = delete;
    void operator=(const Timer&) = delete;

    void updateConfig(const ConfigCounter& config);
    void updateConfig(const ConfigPwm& config);

    void start();
    void stop();

private:
    friend class Board;
    Timer(Registers& Registers);
    ~Timer();
    const Registers& registers_;
};

#define TIMER0_REGISTERS                                                       \
    (Timer<uint8_t>::Registers){.waveformA{Pin::Region::B, Pin::Id::P3},      \
                                 .waveformB{Pin::Region::B, Pin::Id::P4},      \
                                 .counter = &TCNT0,                            \
                                 .compareA = &OCR0A,                           \
                                 .compareB = &OCR0B,                           \
                                 .controlA = &TCCR0A,                          \
                                 .controlB = &TCCR0B,                          \
                                 .controlC = nullptr,                          \
                                 .interruptMask = &TIMSK0}

#define TIMER1_REGISTERS                                                       \
    ((Timer<uint16_t>::Registers){.waveformA{Pin::Region::D, Pin::Id::P5},     \
                                  .waveformB{Pin::Region::D, Pin::Id::P4},     \
                                  .counter = &TCNT1,                           \
                                  .compareA = &OCR1A,                          \
                                  .compareB = &OCR1B,                          \
                                  .controlA = &TCCR1A,                         \
                                  .controlB = &TCCR1B,                         \
                                  .controlC = &TCCR1C,                         \
                                  .interruptMask = &TIMSK1})

#define TIMER2_REGISTERS                                                       \
    ((Timer<uint8_t>::Registers){.waveformA{Pin::Region::D, Pin::Id::P7},      \
                                 .waveformB{Pin::Region::D, Pin::Id::P6},      \
                                 .counter = &TCNT2,                            \
                                 .compareA = &OCR2A,                           \
                                 .compareB = &OCR2B,                           \
                                 .controlA = &TCCR2A,                          \
                                 .controlB = &TCCR2B,                          \
                                 .controlC = nullptr,                          \
                                 .interruptMask = &TIMSK2})

#endif /* _TIMER_H */