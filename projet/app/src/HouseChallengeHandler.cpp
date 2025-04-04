#include "HouseChallengeHandler.h"

#include "Challenge.h"

HouseChallengeHandler::HouseChallengeHandler()
    : point_(Point::E_INITIAL), isPolePresent_(false),
      sweepTimeLeftMs_(SWEEP_TIME_MS) {}

HouseChallengeHandler::~HouseChallengeHandler() {}

void HouseChallengeHandler::update(uint16_t deltaTimeMs, Challenge& challenge) {
    LineFollower<uint8_t, TimerPrescalerSynchronous>& lineFollower =
        challenge.getLineFollower();

    if (!lineFollower.isEvent()) {
        return;
    }

    switch (this->point_) {
        case Point::E_INITIAL:
            lineFollower.start();
            this->point_ = Point::F;
            break;
        case Point::F:
            lineFollower.start(
                LineFollowerState::TURNING_RIGHT);
            this->point_ = Point::G;
            break;
        case Point::G:
            if (this->sweepTimeLeftMs_ != 0) {
                this->sweepForPole(deltaTimeMs);
            } else if (this->isPolePresent_) {
                this->point_ = Point::I;
                lineFollower.start(
                    LineFollowerState::
                        TURNING_RIGHT);
            } else {
                this->point_ = Point::H;
                lineFollower.start(
                    LineFollowerState::FORWARD);
            }
            break;
        case Point::H:
            lineFollower.start();
            this->point_ = Point::I;
            break;
        case Point::I:
            lineFollower.start(LineFollowerState::TURNING_RIGHT);
            this->point_ = Point::E_FINAL;
            break;
        case Point::E_FINAL:
            lineFollower.start(LineFollowerState::TURNING_RIGHT);
            this->point_ = Point::F_FINAL;
            break;
        case Point::F_FINAL:
            lineFollower.start();
            challenge.setState(Challenge::State::FINISH);
            break;
    }
}

void HouseChallengeHandler::sweepForPole(uint16_t deltaTimeMs) {
    Robot& robot = Robot::get();

    constexpr uint16_t TIME_OFFSET_MS = SWEEP_TIME_MS / 4;

    if (this->sweepTimeLeftMs_ == 0)
        robot.getMovementManager().stop();
    if (this->sweepTimeLeftMs_ > SWEEP_TIME_MS - TIME_OFFSET_MS
        || this->sweepTimeLeftMs_ < TIME_OFFSET_MS)
        robot.getMovementManager().moveLeft(Challenge::SPEED, 0.0F);
    else
        robot.getMovementManager().moveRight(Challenge::SPEED, 0.0F);

    this->isPolePresent_ |= robot.getDistanceSensor().getDistanceCm() <= 20;

    if (deltaTimeMs <= this->sweepTimeLeftMs_)
        this->sweepTimeLeftMs_ -= deltaTimeMs;
    else
        this->sweepTimeLeftMs_ = 0;
}
