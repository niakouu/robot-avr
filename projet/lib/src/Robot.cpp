#include "Robot.h"

Robot Robot::robot_{};

Robot::Robot() noexcept
    : board_(Board::get()),
      distanceSensor_(Pin::Id::P7),
      bidirectionalLed_(Pin::Region::C, Pin::Id::P7, Pin::Id::P6),
      lineSensor_({
          Pin(Pin::Region::A, Pin::Id::P0), // left
          Pin(Pin::Region::A, Pin::Id::P1), // semi_left
          Pin(Pin::Region::A, Pin::Id::P2), // center
          Pin(Pin::Region::A, Pin::Id::P3), // semi_right
          Pin(Pin::Region::A, Pin::Id::P4)  // right
      }),
      midi_(Pin::Region::D, Pin::Id::P4),
      button_(Pin::Region::D, Pin::Id::P2, true) {}

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
  return this->button_;
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

Button& Robot::getButton() {
  return this->button_;
}