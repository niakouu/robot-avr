#ifndef _BOARD_H
#define _BOARD_H

#include "Adc.h"
#include "Button.h"
#include "Memory24.h"
#include "MovementManager.h"
#include "Timer.h"
#include "Uart.h"
#include "WatchdogTimer.h"

class Board {
public:
    static const Board& getBoard();

    const Timer<uint8_t>& getTimer0() const;
    const Timer<uint8_t>& getTimer2() const;
    const Timer<uint16_t>& getTimer1() const;
    const Adc& getAdc() const;
    const Memory24CXXX& getMemory() const;
    const Uart& getUart0() const;
    const Button& getButton() const;
    const MovementManager<uint8_t>& getMovementManager() const;
    const WatchdogTimer& getWatchdogTimer() const;

private:
    static Board board_;
    Timer<uint8_t> timer0_, timer2_;
    Timer<uint16_t> timer1_;
    Adc adc_;
    Memory24CXXX memory_;
    Uart uart0_;
    Button button_;
    MovementManager<uint8_t> movementManager_;
    WatchdogTimer watchdogTimer_;
    Board();
    ~Board();
};

#endif /* _BOARD_H */