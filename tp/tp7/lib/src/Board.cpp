#include "Board.h"

Board::Board() {
    // TODO
}

Board::~Board() {
    // TODO
}

const Board& Board::getBoard() {
    // TODO: insert return statement here
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