#include "Board.h"

Board Board::board_{};

Board::Board() noexcept
    : timer0_(TimerConstants::TIMER0_REGISTERS),
      timer1_(TimerConstants::TIMER1_REGISTERS),
      timer2_(TimerConstants::TIMER2_REGISTERS), uart0_(USART0_REGISTERS),
      button_(Button::Interrupt::I0, true) {}

Board::~Board() = default;

Board& Board::get() {
    return Board::board_;
}

const Timer0& Board::getTimer0() const {
    return this->timer0_;
}

const Timer1& Board::getTimer1() const {
    return this->timer1_;
}

const Timer2& Board::getTimer2() const {
    return this->timer2_;
}

const Adc& Board::getAdc() const {
    return this->adc_;
}

const Memory24& Board::getMemory() const {
    return this->memory_;
}

const Uart& Board::getUart0() const {
    return this->uart0_;
}

const Button& Board::getButton() const {
    return this->button_;
}

const WatchdogTimer& Board::getWatchdogTimer() const {
    return this->watchdogTimer_;
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

Memory24& Board::getMemory() {
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