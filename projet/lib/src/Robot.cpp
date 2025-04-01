#include "Robot.h"

Robot Robot::robot_{};

Robot::Robot() noexcept
    : board_(Board::get()), distanceSensor_(Pin::Id::P7),
      bidirectionalLed_(Pin::Region::C, Pin::Id::P7, Pin::Id::P6),
      lineSensor_({.left = Pin(Pin::Region::A, Pin::Id::P0),
                   .semiLeft = Pin(Pin::Region::A, Pin::Id::P1),
                   .center = Pin(Pin::Region::A, Pin::Id::P2),
                   .semiRight = Pin(Pin::Region::A, Pin::Id::P3),
                   .right = Pin(Pin::Region::A, Pin::Id::P4)}),
      midi_(Pin::Region::D, Pin::Id::P4),
      button_(Pin::Region::D, Pin::Id::P2, true),
      movementManager_(Board::get().getTimer1(),
                       {Pin(Pin::Region::D, Pin::Id::P5, Pin::Direction::OUT),
                        0},
                       {Pin(Pin::Region::D, Pin::Id::P4, Pin::Direction::OUT),
                        0}) {}

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

template <typename T, typename U>
const MovementManager<T, U>& Robot::getMovementManager() const {
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

Button& Robot::getButton() {
    return this->button_;
}

template <typename T, typename U>
MovementManager<T, U>& Robot::getMovementManager() {
    return this->movementManager_;
}

void Robot::followLine() {
    float leftCurve = 0;
    float rightCurve = 0;
    uint8_t leftCounter = 0;
    uint8_t rightCounter = 0;
    float speed = 1.0f;

    if (lineSensor_.isDark(LineSensor::Direction::LEFT)) {
        leftCurve += 70;
        leftCounter++;
    }

    if (lineSensor_.isDark(LineSensor::Direction::SEMI_LEFT)) {
        leftCurve += 35;
        leftCounter++;
    }

    if (lineSensor_.isDark(LineSensor::Direction::CENTER)) {
        leftCounter++;
        rightCounter++;
    }

    if (lineSensor_.isDark(LineSensor::Direction::SEMI_RIGHT)) {
        rightCurve += 35;
        rightCounter++;
    }

    if (lineSensor_.isDark(LineSensor::Direction::RIGHT)) {
        rightCurve += 70;
        rightCounter++;
    }

    leftCurve = leftCurve / leftCounter;
    rightCurve = rightCurve / rightCounter;

    if (leftCurve > rightCurve) {
        movementManager_.moveLeft(speed, ((leftCurve - rightCurve) / 360));
    } else if (leftCurve < rightCurve) {
        movementManager_.moveRight(speed, ((rightCurve - leftCurve) / 360));
    } else if (leftCurve == 0 && rightCurve == 0) {
        movementManager_.moveForward(speed);
    } else {
        movementManager_.moveForward(0.0f);
    }
}
