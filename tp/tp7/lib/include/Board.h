#ifndef _BOARD_H
#define _BOARD_H

#include "Adc.h"
#include "Button.h"
#include "Memory24.h"
#include "Motor.h"
#include "MovementManager.h"
#include "Timer.h"
#include "Uart.h"
#include "WatchdogTimer.h"

class Board {
public:
    static Board& get();
    Board(Board&) = delete;
    void operator=(const Board&) = delete;

    const Timer0& getTimer0() const;
    const Timer1& getTimer1() const;
    const Timer2& getTimer2() const;
    const Adc& getAdc() const;
    const Memory24CXXX& getMemory() const;
    const Uart& getUart0() const;
    const Button& getButton() const;
    const WatchdogTimer& getWatchdogTimer() const;

    Timer0& getTimer0();
    Timer1& getTimer1();
    Timer2& getTimer2();
    Adc& getAdc();
    Memory24CXXX& getMemory();
    Uart& getUart0();
    Button& getButton();
    WatchdogTimer& getWatchdogTimer();

private:
    static Board board_;
    Timer0 timer0_;
    Timer1 timer1_;
    Timer2 timer2_;
    Adc adc_;
    Memory24CXXX memory_;
    Uart uart0_;
    Button button_;
    WatchdogTimer watchdogTimer_;
    Board() noexcept;
    ~Board();
};

#endif /* _BOARD_H */