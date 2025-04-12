#include "Robot.h"

#include <math.h>

Robot Robot::robot_{};

Robot::Robot() noexcept
    : board_(Board::get()), distanceSensor_(Pin::Id::P7),
      bidirectionalLed_(Pin::Region::C, Pin::Id::P7, Pin::Id::P6),
      lineSensor_({.left = Pin(Pin::Region::A, Pin::Id::P0),
                   .semiLeft = Pin(Pin::Region::A, Pin::Id::P1),
                   .center = Pin(Pin::Region::A, Pin::Id::P2),
                   .semiRight = Pin(Pin::Region::A, Pin::Id::P3),
                   .right = Pin(Pin::Region::A, Pin::Id::P4)}),
      midi_(Pin::Region::D, Pin::Id::P7),
      extraButton_{Button::Interrupt::I1, false},
      movementManager_(Board::get().getTimer0(),
                       {Pin(Pin::Region::B, Pin::Id::P2, Pin::Direction::OUT),
                        OFFSET_LEFT},
                       {Pin(Pin::Region::B, Pin::Id::P5, Pin::Direction::OUT),
                        OFFSET_RIGHT}) {}

Robot::~Robot() = default;

Robot& Robot::get() {
    return Robot::robot_;
}

const Board& Robot::getBoard() const {
    return this->board_;
}

const DistanceSensor& Robot::getDistanceSensor() const {
    return this->distanceSensor_;
}

const BidirectionalLed& Robot::getBidirectionalLed() const {
    return this->bidirectionalLed_;
}

const LineSensor& Robot::getLineSensor() const {
    return this->lineSensor_;
}

const Midi& Robot::getMidi() const {
    return this->midi_;
}

const Button& Robot::getButton() const {
    return this->board_.getButton();
}
const Button& Robot::getExtraButton() const {
    return this->extraButton_;
}


const MovementManager<uint8_t, TimerPrescalerSynchronous>&
Robot::getMovementManager() const {
    return this->movementManager_;
}

Board& Robot::getBoard() {
    return this->board_;
}

DistanceSensor& Robot::getDistanceSensor() {
    return this->distanceSensor_;
}

BidirectionalLed& Robot::getBidirectionalLed() {
    return this->bidirectionalLed_;
}

LineSensor& Robot::getLineSensor() {
    return this->lineSensor_;
}

Midi& Robot::getMidi() {
    return this->midi_;
}

 Button& Robot::getButton()  {
    return this->board_.getButton();
}


Button& Robot::getExtraButton() {
    return this->extraButton_;
}


MovementManager<uint8_t, TimerPrescalerSynchronous>&
Robot::getMovementManager() {
    return this->movementManager_;
}