#ifndef _BOARD_H
#define _BOARD_H

#include "Adc.h"
#include "Button.h"
#include "Memory24.h"
#include "MovementManager.h"
#include "Timer.h"
#include "Uart.h"
#include "WatchdogTimer.h"
#include "Motor.h"

class Board {
public:
    static const Board& get();
    Board(Board&) = delete;
    void operator=(const Board&) = delete;

    const Timer<uint8_t, TimerPrescalerSynchronous>& getTimer0() const;
    const Timer<uint16_t, TimerPrescalerSynchronous>& getTimer1() const;
    const Timer<uint8_t, TimerPrescalerAsynchronous>& getTimer2() const;
    const Adc& getAdc() const;
    const Memory24CXXX& getMemory() const;
    const Uart& getUart0() const;
    const Button& getButton() const;
    const WatchdogTimer& getWatchdogTimer() const;

private:
    static Board board_;
    Timer<uint8_t, TimerPrescalerSynchronous> timer0_;
    Timer<uint16_t, TimerPrescalerSynchronous> timer1_;
    Timer<uint8_t, TimerPrescalerAsynchronous> timer2_;
    Adc adc_;
    Memory24CXXX memory_;
    Uart uart0_;
    Button button_;
    WatchdogTimer watchdogTimer_;
    Board() noexcept;
    ~Board();
};

#endif /* _BOARD_H */