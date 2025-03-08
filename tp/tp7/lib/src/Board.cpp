#include "Board.h"

Board Board::board_{};

Board::Board() noexcept
    : timer0_(TIMER0_REGISTERS), timer1_(TIMER1_REGISTERS),
      timer2_(TIMER2_REGISTERS), uart0_(USART0_REGISTERS),
      button_(Pin::Region::D, Pin::Id::P2, true) {}

Board::~Board() = default;

const Board& Board::get() {
    return Board::board_;
}

const Timer<uint8_t, TimerPrescalerSynchronous>& Board::getTimer0() const {
    return timer0_;
}

const Timer<uint16_t, TimerPrescalerSynchronous>& Board::getTimer1() const {
    return timer1_;
}

const Timer<uint8_t, TimerPrescalerAsynchronous>& Board::getTimer2() const {
    return timer2_;
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

const WatchdogTimer& Board::getWatchdogTimer() const {
    return watchdogTimer_;
}