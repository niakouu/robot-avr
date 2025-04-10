#include "MazeChallengeHandler.h"

#include "Challenge.h"
#include "Robot.h"

MazeChallengeHandler::MazeChallengeHandler()
    : currentPoint_(Point::ENTRY_ONE), rotateTimeLeftMs_(TURN_TIME_MS),
      averagePoleDistance_(0), counter_(0), totalReadings_(POLE_READING_COUNT),
      finishedCalculatingPole_(false) {}

void MazeChallengeHandler::update(uint16_t deltaTimeMs, Challenge& challenge) {
    LineFollower<uint8_t, TimerPrescalerSynchronous>& lineFollower =
        challenge.getLineFollower();

    if (!lineFollower.isLost())
        return;

    LineFollowerConfiguration configuration{.isAutomatic = true,
                                            .isEventOnThree = true,
                                            .isTurnInPlace = false,
                                            .isSkippingLine = true};

    switch (this->currentPoint_) {
        case Point::ENTRY_ONE:
            execFirstDecision(deltaTimeMs, configuration, Point::L,
                              Point::ENTRY_TWO);
            break;
        case Point::ENTRY_TWO:
            execSecondDecision(configuration, Point::ENTRY_TO_M,
                               Point::ENTRY_THREE);
            break;
        case Point::ENTRY_THREE:
            configuration.isTurnInPlace = true;
            switchPoint(Point::ENTRY_TO_K, LineFollowerState::TURNING_RIGHT,
                        configuration);
            break;
        case Point::ENTRY_TO_K:
            switchPoint(Point::K, LineFollowerState::TURNING_LEFT,
                        configuration);
            break;
        case Point::ENTRY_TO_M:
            switchPoint(Point::M, LineFollowerState::TURNING_RIGHT,
                        configuration);
            break;
        case Point::K:
            switchPoint(Point::BETWEEN_K_N, LineFollowerState::FORWARD,
                        configuration);
            break;
        case Point::L:
            switchPoint(Point::SECOND_ENTRY_ONE, LineFollowerState::FORWARD,
                        configuration);
            break;
        case Point::M:
            switchPoint(Point::BETWEEN_M_P, LineFollowerState::FORWARD,
                        configuration);
            break;
        case Point::BETWEEN_K_N:
            detectPoleBetweenSidePoints(configuration, Point::N,
                                        Point::SECOND_ENTRY_ONE);
            break;
        case Point::BETWEEN_M_P:
            detectPoleBetweenSidePoints(configuration, Point::P,
                                        Point::SECOND_ENTRY_ONE);
            break;
        case Point::SECOND_ENTRY_ONE:
            execFirstDecision(deltaTimeMs, configuration, Point::O,
                              Point::SECOND_ENTRY_TWO);
            break;
        case Point::SECOND_ENTRY_TWO:
            execSecondDecision(configuration, Point::SECOND_ENTRY_TO_P,
                               Point::SECOND_ENTRY_THREE);
        case Point::SECOND_ENTRY_THREE:
            configuration.isTurnInPlace = true;
            switchPoint(Point::SECOND_ENTRY_TO_N,
                        LineFollowerState::TURNING_RIGHT, configuration);
            break;
        case Point::K_TO_N:
            switchPoint(Point::N, LineFollowerState::FORWARD, configuration);
            break;
        case Point::M_TO_P:
            switchPoint(Point::P, LineFollowerState::FORWARD, configuration);
            break;
        case Point::SECOND_ENTRY_TO_N:
            switchPoint(Point::N, LineFollowerState::TURNING_LEFT,
                        configuration);
            break;
        case Point::SECOND_ENTRY_TO_P:
            switchPoint(Point::P, LineFollowerState::TURNING_RIGHT,
                        configuration);
            break;
        case Point::N:
            switchPoint(Point::LAST, LineFollowerState::FORWARD, configuration);
            this->counter_ = 0;
            break;
        case Point::O:
            switchPoint(Point::LAST, LineFollowerState::FORWARD, configuration);
            this->counter_ = 1;
            break;
        case Point::P:
            switchPoint(Point::LAST, LineFollowerState::FORWARD, configuration);
            this->counter_ = 2;
            break;
        case Point::LAST:
            switchPoint(Point::EXIT, LineFollowerState::TURNING_RIGHT,
                        configuration);
            break;
        case Point::EXIT:
            endingPointHandler(challenge, configuration);
            break;
        default:
            break;
    }
    lineFollower.start(configuration);
}

bool MazeChallengeHandler::rotate(bool left, uint16_t deltaTimeMs,
                                  uint16_t turnTimeMs) {
    Robot& robot = Robot::get();

    if (this->rotateTimeLeftMs_ == 0) {
        robot.getMovementManager().stop();
        this->rotateTimeLeftMs_ = turnTimeMs;
        return true;
    }

    if (this->rotateTimeLeftMs_ == turnTimeMs) {
        if (left)
            robot.getMovementManager().moveLeft(Challenge::SPEED, 0.0F);
        else
            robot.getMovementManager().moveRight(Challenge::SPEED, 0.0F);
    }

    this->rotateTimeLeftMs_ =
        cappingSubtract(this->rotateTimeLeftMs_, deltaTimeMs);
    return false;
}

void MazeChallengeHandler::endingPointHandler(
    Challenge& challenge, LineFollowerConfiguration& configuration) {
    if (counter_ == 0) {
        challenge.setState(Challenge::State::FOLLOW_LINE);
        return;
    }

    configuration.state = LineFollowerState::FORWARD;
    this->counter_--;
}

bool MazeChallengeHandler::isPolePresent(uint8_t distance) const {
    return this->averagePoleDistance_ <= distance;
}

void MazeChallengeHandler::calculatePoleDistance() {
    if (this->totalReadings_ != 0) {
        --this->totalReadings_;
        this->averagePoleDistance_ +=
            Robot::get().getDistanceSensor().getDistanceCm();
        return;
    }

    this->averagePoleDistance_ /= POLE_READING_COUNT;
    this->finishedCalculatingPole_ = true;
}

void MazeChallengeHandler::resetDistanceValues() {
    this->averagePoleDistance_ = 0;
    this->totalReadings_ = POLE_READING_COUNT;
    this->finishedCalculatingPole_ = false;
}

void MazeChallengeHandler::execFirstDecision(
    uint16_t deltaTimeMs, LineFollowerConfiguration& configuration,
    Point forward, Point currentNext) {
    if (!this->finishedCalculatingPole_) {
        calculatePoleDistance();
        return;
    }

    configuration.isTurnInPlace = true;

    if (!isPolePresent(DISTANCE_TO_CENTER)) {
        resetDistanceValues();
        configuration.state = LineFollowerState::FORWARD;
        this->currentPoint_ = forward;
    } else if (rotate(true, deltaTimeMs, TURN_TIME_MS)) {
        resetDistanceValues();
        configuration.state = LineFollowerState::LOST;
        this->currentPoint_ = currentNext;
    }
}

void MazeChallengeHandler::execSecondDecision(
    LineFollowerConfiguration& configuration, Point left, Point currentNext) {
    if (!this->finishedCalculatingPole_) {
        calculatePoleDistance();
        return;
    }

    configuration.isTurnInPlace = true;

    const bool leftPolePresence = isPolePresent(DISTANCE_TO_DIAGONAL);
    configuration.isAutomatic = !leftPolePresence;
    configuration.state = leftPolePresence ? LineFollowerState::TURNING_RIGHT
                                           : LineFollowerState::TURNING_LEFT;
    this->currentPoint_ = leftPolePresence ? currentNext : left;

    resetDistanceValues();
}

void MazeChallengeHandler::detectPoleBetweenSidePoints(
    LineFollowerConfiguration& configuration, Point forward, Point side) {
    if (!this->finishedCalculatingPole_) {
        calculatePoleDistance();
        return;
    }

    const bool polePresence = isPolePresent(DISTANCE_TO_CENTER);
    configuration.state = polePresence ? LineFollowerState::TURNING_LEFT
                                       : LineFollowerState::FORWARD;
    this->currentPoint_ = polePresence ? side : forward;

    resetDistanceValues();
}

void MazeChallengeHandler::switchPoint(
    Point nextPoint, LineFollowerState nextLineState,
    LineFollowerConfiguration& configuration) {
    configuration.state = nextLineState;
    this->currentPoint_ = nextPoint;
}
