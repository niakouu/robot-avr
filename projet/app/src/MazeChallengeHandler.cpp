#include "MazeChallengeHandler.h"

#include "Challenge.h"
#include "Robot.h"

MazeChallengeHandler::MazeChallengeHandler()
    : currentState_(isPolePresent(DISTANCE_TO_CENTER)
                        ? MazeChallengeHandler::Point::ENTRY
                        : MazeChallengeHandler::Point::L),
      rotateTimeLeftMs_(SWEEP_TIME_MS), sweepTimeLeftMs_(SWEEP_TIME_MS) {}

void MazeChallengeHandler::update(uint16_t deltaTimeMs, Challenge& challenge) {
    LineFollower<uint8_t, TimerPrescalerSynchronous>& lineFollower =
        challenge.getLineFollower();

    if (!lineFollower.isLost()) {
        return;
    }

    switch (this->currentState_) {
        case MazeChallengeHandler::Point::ENTRY:
            determinePointState(deltaTimeMs, MazeChallengeHandler::Point::M,
                                MazeChallengeHandler::Point::K);
            break;
        case MazeChallengeHandler::Point::K:
            firstStagePointHandler(true, lineFollower,
                                   MazeChallengeHandler::Point::SECOND_ENTRY);
            break;
        case MazeChallengeHandler::Point::L:
            pointLHandler(lineFollower);
            break;
        case MazeChallengeHandler::Point::M:
            firstStagePointHandler(false, lineFollower,
                                   MazeChallengeHandler::Point::SECOND_ENTRY);
            break;
        case MazeChallengeHandler::Point::SECOND_ENTRY:
            if (isPolePresent(DISTANCE_TO_CENTER)) {
                this->currentState_ = MazeChallengeHandler::Point::O;
                this->counter_ = 2;
                break;
            }
            determinePointState(deltaTimeMs, MazeChallengeHandler::Point::P,
                                MazeChallengeHandler::Point::N);
            break;
        case MazeChallengeHandler::Point::N:
        case MazeChallengeHandler::Point::O:
            secondStagePointHandler(lineFollower);
            break;
        case MazeChallengeHandler::Point::P:
            firstStagePointHandler(false, lineFollower,
                                   MazeChallengeHandler::Point::SECOND_ENTRY);
            break;
        case MazeChallengeHandler::Point::EXIT:
            endingPointHandler(challenge, lineFollower);
            break;
        default:
            break;
    }
}

void MazeChallengeHandler::determinePointState(uint16_t deltaTimeMs,
                                               Point leftPoint,
                                               Point rightPoint) {
    const bool left =
        this->sweepTimeLeftMs_ > SWEEP_TIME_MS - SWEEP_TIME_OFFSET_MS;

    rotate(left, deltaTimeMs, SWEEP_TIME_MS);
    if (isPolePresent(DISTANCE_TO_DIAGONAL)) {
        this->currentState_ = left ? leftPoint : rightPoint;
        this->rotateTimeLeftMs_ = SWEEP_TIME_OFFSET_MS;
        this->sweepTimeLeftMs_ = SWEEP_TIME_MS;
        this->counter_ = FIRST_LOST_COUNTER;
        return;
    }

    if (deltaTimeMs <= this->sweepTimeLeftMs_)
        this->sweepTimeLeftMs_ -= deltaTimeMs;
    else
        this->sweepTimeLeftMs_ = 0;
}

void MazeChallengeHandler::rotate(bool left, uint16_t deltaTimeMs,
                                  uint16_t turnTimeMs) {
    Robot& robot = Robot::get();

    if (this->rotateTimeLeftMs_ == 0) {
        robot.getMovementManager().stop();
        this->rotateTimeLeftMs_ = turnTimeMs;
        return;
    }
    if (this->rotateTimeLeftMs_ == turnTimeMs) {
        if (left)
            robot.getMovementManager().moveLeft(Challenge::SPEED, 0.0F);
        else
            robot.getMovementManager().moveRight(Challenge::SPEED, 0.0F);
    }

    if (deltaTimeMs <= this->rotateTimeLeftMs_)
        this->rotateTimeLeftMs_ -= deltaTimeMs;
    else
        this->rotateTimeLeftMs_ = 0;
}

void MazeChallengeHandler::firstStagePointHandler(
    bool upperPoint,
    LineFollower<uint8_t, TimerPrescalerSynchronous>& lineFollower,
    Point nextPoint) {

    if (this->counter_ == 0) {
        if (this->currentState_ == MazeChallengeHandler::Point::P)
            this->counter_ = 3;
        this->currentState_ = nextPoint;
        return;
    }

    if (this->counter_ == 3 || this->counter_ == 1)
        lineFollower.start(upperPoint ? LineFollowerState::TURNING_LEFT
                                      : LineFollowerState::TURNING_RIGHT);
    else if (this->counter_ == 2)
        lineFollower.start();

    this->counter_--;
}

void MazeChallengeHandler::secondStagePointHandler(
    LineFollower<uint8_t, TimerPrescalerSynchronous>& lineFollower) {
    if (this->counter_ == 0) {
        this->currentState_ = MazeChallengeHandler::Point::EXIT;
        return;
    }

    if (this->counter_ == 3)
        lineFollower.start(LineFollowerState::TURNING_LEFT);
    else if (this->counter_ == 2)
        lineFollower.start();
    else if (this->counter_ == 1)
        lineFollower.start(LineFollowerState::TURNING_RIGHT);

    this->counter_--;
}

void MazeChallengeHandler::pointLHandler(
    LineFollower<uint8_t, TimerPrescalerSynchronous>& lineFollower) {
    if (this->counter_ == 1) {
        this->currentState_ = MazeChallengeHandler::Point::SECOND_ENTRY;
        return;
    }

    lineFollower.start();
    this->counter_++;
}

void MazeChallengeHandler::endingPointHandler(
    Challenge& challenge,
    LineFollower<uint8_t, TimerPrescalerSynchronous>& lineFollower) {
    if (counter_ == 0) {
        challenge.setState(Challenge::State::FOLLOW_LINE);
        return;
    }
    
    lineFollower.start();
    this->counter_--;
}

bool MazeChallengeHandler::isPolePresent(uint8_t distance) {
    return Robot::get().getDistanceSensor().getDistanceCm() <= distance;
}
