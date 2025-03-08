#include "Board.h"

Board Board::board_{};

Board::Board() noexcept
    : timer0_(TIMER0_REGISTERS), timer1_(TIMER1_REGISTERS),
      timer2_(TIMER2_REGISTERS), uart0_(USART0_REGISTERS),
      button_(Pin::Region::D, Pin::Id::P2, true) {}

Board::~Board() = default;

Board& Board::get() {
    return Board::board_;
}

const Timer0& Board::getTimer0() const {
    return timer0_;
}

const Timer1& Board::getTimer1() const {
    return timer1_;
}

const Timer2& Board::getTimer2() const {
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

Timer0& Board::getTimer0() {
    return this->timer0_;
}

Timer1& Board::getTimer1() {
    return this->timer1_;
}

Timer2& Board::getTimer2() {
    return this->timer2_;
}

Adc& Board::getAdc() {
    return this->adc_;
}

Memory24CXXX& Board::getMemory() {
    return this->memory_;
}

Uart& Board::getUart0() {
    return this->uart0_;
}

Button& Board::getButton() {
    return this->button_;
}

WatchdogTimer& Board::getWatchdogTimer() {
    return this->watchdogTimer_;
}
