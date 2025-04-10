#include "MazeChallengeHandler.h"

#include "Challenge.h"
#include "Robot.h"

MazeChallengeHandler::MazeChallengeHandler()
    : currentPoint_(Point::ENTRY_ONE), rotateTimeLeftMs_(TURN_TIME_MS),
      averagePoleDistance_(0), counter_(0), totalReadings_(POLE_READING_COUNT),
      finishedCalculatingPole_(false) {}

const char* MazeChallengeHandler::toString(Point point) { // TODO REMOVE
    switch (point) {
        case Point::ENTRY_ONE:
            return "ENTRY_ONE";
        case Point::ENTRY_TWO:
            return "ENTRY_TWO";
        case Point::ENTRY_THREE:
            return "ENTRY_THREE";
        case Point::ENTRY_TO_K:
            return "ENTRY_TO_K";
        case Point::ENTRY_TO_M:
            return "ENTRY_TO_M";
        case Point::K:
            return "K";
        case Point::L:
            return "L";
        case Point::M:
            return "M";
        case Point::BETWEEN_K_N:
            return "BETWEEN_K_N";
        case Point::BETWEEN_M_P:
            return "BETWEEN_M_P";
        case Point::K_TO_N:
            return "K_TO_N";
        case Point::M_TO_P:
            return "M_TO_P";
        case Point::SECOND_ENTRY_ONE:
            return "SECOND_ENTRY_ONE";
        case Point::SECOND_ENTRY_TWO:
            return "SECOND_ENTRY_TWO";
        case Point::SECOND_ENTRY_THREE:
            return "SECOND_ENTRY_THREE";
        case Point::SECOND_ENTRY_TO_N:
            return "SECOND_ENTRY_TO_N";
        case Point::SECOND_ENTRY_TO_P:
            return "SECOND_ENTRY_TO_P";
        case Point::N:
            return "N";
        case Point::O:
            return "O";
        case Point::P:
            return "P";
        case Point::LAST:
            return "LAST";
        case Point::EXIT:
            return "EXIT";
        default:
            return "UNKNOWN";
    }
}

const char* toString(LineFollowerState state) {
    switch (state) {
        case LineFollowerState::FORWARD:
            return "FORWARD";
        case LineFollowerState::DETECTION:
            return "DETECTION";
        case LineFollowerState::TURNING_LEFT:
            return "TURNING_LEFT";
        case LineFollowerState::TURNING_RIGHT:
            return "TURNING_RIGHT";
        case LineFollowerState::LOST:
            return "LOST";
        case LineFollowerState::STOP:
            return "STOP";
        default:
            return "UNKNOWN";
    }
}

void MazeChallengeHandler::update(uint16_t deltaTimeMs, Challenge& challenge) {
    LineFollower<uint8_t, TimerPrescalerSynchronous>& lineFollower =
        challenge.getLineFollower();

    if (!lineFollower.isLost())
        return;

    LineFollowerConfiguration configuration{.state = LineFollowerState::LOST,
                                            .isAutomatic = true,
                                            .isEventOnThree = true,
                                            .isTurnInPlace = false,
                                            .isSkippingLine = true};

    printf("%s -> ", toString(this->currentPoint_));
    bool isMotorManual = false;
    switch (this->currentPoint_) {
        case Point::ENTRY_ONE:
            isMotorManual = execFirstDecision(deltaTimeMs, configuration,
                                              Point::L, Point::ENTRY_TWO);
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
            isMotorManual =
                execFirstDecision(deltaTimeMs, configuration, Point::O,
                                  Point::SECOND_ENTRY_TWO);
            break;
        case Point::SECOND_ENTRY_TWO:
            execSecondDecision(configuration, Point::SECOND_ENTRY_TO_P,
                               Point::SECOND_ENTRY_THREE);
            break;
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

    printf("%s %s a:%d e3:%d inplace:%d skip:%d\n",
           toString(this->currentPoint_), ::toString(configuration.state),
           configuration.isAutomatic, configuration.isEventOnThree,
           configuration.isTurnInPlace, configuration.isSkippingLine);
    if (!isMotorManual)
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
    printf("avg dist: %d\n", averagePoleDistance_);
    this->finishedCalculatingPole_ = true;
}

void MazeChallengeHandler::resetDistanceValues() {
    this->averagePoleDistance_ = 0;
    this->totalReadings_ = POLE_READING_COUNT;
    this->finishedCalculatingPole_ = false;
}

bool MazeChallengeHandler::execFirstDecision(
    uint16_t deltaTimeMs, LineFollowerConfiguration& configuration,
    Point forward, Point currentNext) {
    if (!this->finishedCalculatingPole_) {
        calculatePoleDistance();
        return false;
    }

    configuration.isTurnInPlace = true;

    if (!isPolePresent(DISTANCE_TO_CENTER)) {
        resetDistanceValues();
        configuration.state = LineFollowerState::FORWARD;
        this->currentPoint_ = forward;
        return false;
    }

    if (rotate(true, deltaTimeMs, TURN_TIME_MS)) {
        resetDistanceValues();
        configuration.state = LineFollowerState::LOST;
        this->currentPoint_ = currentNext;
        return false;
    }

    return true;
}

void MazeChallengeHandler::execSecondDecision(
    LineFollowerConfiguration& configuration, Point left, Point currentNext) {
    if (!this->finishedCalculatingPole_) {
        calculatePoleDistance();
        return;
    }

    configuration.isTurnInPlace = false;
    configuration.isSkippingLine = false;

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

    if (polePresence) {
        configuration.state = this->currentPoint_ == Point::BETWEEN_K_N
                                  ? LineFollowerState::TURNING_LEFT
                                  : LineFollowerState::TURNING_RIGHT;
    } else {
        configuration.state = LineFollowerState::FORWARD;
    }

    this->currentPoint_ = polePresence ? side : forward;

    resetDistanceValues();
}

void MazeChallengeHandler::switchPoint(
    Point nextPoint, LineFollowerState nextLineState,
    LineFollowerConfiguration& configuration) {
    configuration.state = nextLineState;
    this->currentPoint_ = nextPoint;
}
