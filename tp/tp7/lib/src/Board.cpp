#include "Board.h"

Board::Board()
    : timer0_(TIMER0_REGISTERS), timer1_(TIMER1_REGISTERS),
      timer2_(TIMER2_REGISTERS), adc_(), memory_(), uart0_(),
      button_(Pin::Region::D, Pin::Id::P2, true), movementManager_(),
      watchdogTimer_() {}

Board::~Board() {}

const Board& Board::getBoard() {
    return Board::board_;
}

const Timer<uint8_t>& Board::getTimer0() const {
    return timer0_;
}

const Timer<uint8_t>& Board::getTimer2() const {
    return timer2_;
}

const Timer<uint16_t>& Board::getTimer1() const {
    return timer1_;
}

const Adc& Board::getAdc() const {
    return adc_;
}

const Memory24CXXX& Board::getMemory() const {
    return memory_;
}

const Uart& Board::getUart0() const {
    return uart0_;
}

const Button& Board::getButton() const {
    return button_;
}

const MovementManager<uint8_t>& Board::getMovementManager() const {
    return movementManager_;
}

const WatchdogTimer& Board::getWatchdogTimer() const {
    return watchdogTimer_;
}