#include "MazeChallengeHandler.h"

#include "Challenge.h"
#include "Robot.h"

MazeChallengeHandler::MazeChallengeHandler()
    : currentPoint_(Point::ENTRY_ONE), rotateTimeLeftMs_(TURN_TIME_MS),
      averagePoleDistance_(0), counter_(0), totalReadings_(POLE_READING_COUNT),
      finishedCalculatingPole_(false), poleMap_{false, false, false},
      currentStage(Stage::Stage1), orientation_(Orientation::Forward),
      lane_(Lane::Center), isIntermediateStep_(false),
      isCheckingInPosition_(false) {}

const char* toString(MazeChallengeHandler::Stage stage) {
    switch (stage) {
        case MazeChallengeHandler::Stage::Stage1:
            return "Stage1";
        case MazeChallengeHandler::Stage::Stage2:
            return "Stage2";
        case MazeChallengeHandler::Stage::End:
            return "End";
        default:
            return "UNKNOWN";
    }
}

const char* toString(MazeChallengeHandler::Lane lane) {
    switch (lane) {
        case MazeChallengeHandler::Lane::Left:
            return "Left";
        case MazeChallengeHandler::Lane::Center:
            return "Center";
        case MazeChallengeHandler::Lane::Right:
            return "Right";
        case MazeChallengeHandler::Lane::Invalid:
            return "Invalid";
        default:
            return "UNKNOWN";
    }
}

const char* toString(MazeChallengeHandler::Orientation orientation) {
    switch (orientation) {
        case MazeChallengeHandler::Orientation::Left:
            return "Left";
        case MazeChallengeHandler::Orientation::Forward:
            return "Forward";
        case MazeChallengeHandler::Orientation::Right:
            return "Right";
        default:
            return "UNKNOWN";
    }
}

void MazeChallengeHandler::update(uint16_t deltaTimeMs, Challenge& challenge) {
    LineFollower<uint8_t, TimerPrescalerSynchronous>& lineFollower =
        challenge.getLineFollower();

    if (!lineFollower.isLost())
        return;

    printf("s:%s l:%s o:%s f:%d%d%d (%s) i:%d\n",
           ::toString(this->currentStage), ::toString(this->lane_),
           ::toString(this->orientation_), this->poleMap_.left,
           this->poleMap_.center, this->poleMap_.right,
           toString(this->getFreeLane()), this->isIntermediateStep_);

    LineFollowerConfiguration configuration{.state = LineFollowerState::LOST,
                                            .isAutomatic = true,
                                            .isEventOnThree = true,
                                            .isTurnInPlace = false,
                                            .isSkippingStartingLine = true};

    if (this->currentStage != Stage::End) {
        if (!this->poleMap_.left && !this->poleMap_.center
            && !this->poleMap_.right) {
            if (!handleNoPoles())
                return;
        }

        const Lane freeLane = this->getFreeLane();

        if (this->lane_ == freeLane) {
            printf("move on this one!!\n");
            this->handleCurrentLaneIsFreeLane(configuration);
        } else if (
            // We move one lane over
            // clang-format off
            (this->lane_ == Lane::Center && (
                freeLane == Lane::Left || freeLane == Lane::Right)) ||
            (freeLane == Lane::Center && (
                this->lane_ == Lane::Left || this->lane_ == Lane::Right))
            // clang-format on
        ) {
            printf("move one over\n");
            this->handleNextLaneIsFreeLane(configuration);
        } else if (
            // Far lane is free lane
            (this->lane_ == Lane::Left && freeLane == Lane::Right)
            || (this->lane_ == Lane::Right && freeLane == Lane::Left)) {
            this->handleFarLaneIsFreeLane(configuration);
            printf("move two over\n");
        } else if (freeLane == Lane::Invalid) {
            if (!this->handleCheckNextLane(configuration, deltaTimeMs))
                return;
        }
    } else {
        configuration.state = this->orientation_ == Orientation::Forward
                                  ? LineFollowerState::TURNING_RIGHT
                                  : LineFollowerState::FORWARD;
    }

    this->updateOrientation(configuration.state);
    printf("    -> s:%d o:%s auto;%d inplace:%d e3:%d skip:%d\n", static_cast<uint8_t>(configuration.state),
           toString(this->orientation_), configuration.isAutomatic, configuration.isTurnInPlace, configuration.isEventOnThree, configuration.isSkippingStartingLine);

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

MazeChallengeHandler::Lane MazeChallengeHandler::getFreeLane() const {
    if (!this->poleMap_.left && this->poleMap_.center && this->poleMap_.right)
        return Lane::Left;

    if (this->poleMap_.left && !this->poleMap_.center && this->poleMap_.right)
        return Lane::Center;

    if (this->poleMap_.left && this->poleMap_.center && !this->poleMap_.right)
        return Lane::Right;

    return Lane::Invalid;
}

bool MazeChallengeHandler::handleNoPoles() {
    if (!this->finishedCalculatingPole_) {
        calculatePoleDistance();
        return false;
    }

    const bool isPolePresent = this->isPolePresent(DISTANCE_TO_CENTER);

    switch (this->lane_) {
        case Lane::Left:
            this->poleMap_.left = isPolePresent;
            this->poleMap_.center = !isPolePresent;
            this->poleMap_.right = !isPolePresent;
            break;
        case Lane::Center:
            this->poleMap_.left = !isPolePresent;
            this->poleMap_.center = isPolePresent;
            this->poleMap_.right = !isPolePresent;
            break;
        case Lane::Right:
            this->poleMap_.left = !isPolePresent;
            this->poleMap_.center = !isPolePresent;
            this->poleMap_.right = isPolePresent;
            break;
        default:
            break;
    }

    resetDistanceValues();

    return true;
}

bool MazeChallengeHandler::handleCheckNextLane(
    LineFollowerConfiguration& configuration, uint16_t deltaTimeMs) {
    const bool isCheckingLeft = this->lane_ != Lane::Left;

    if (!this->isCheckingInPosition_) {
        if (rotate(isCheckingLeft, deltaTimeMs, TURN_TIME_MS)) {
            resetDistanceValues();
            this->isCheckingInPosition_ = true;
        } else {
            return false;
        }
    }

    if (!this->finishedCalculatingPole_) {
        calculatePoleDistance();
        return false;
    }

    const bool isPolePresent = this->isPolePresent(DISTANCE_TO_DIAGONAL);

    switch (this->lane_) {
        case Lane::Left:
            this->poleMap_.center = isPolePresent;
            this->poleMap_.right = !isPolePresent;
            break;
        case Lane::Center:
            this->poleMap_.left = isPolePresent;
            this->poleMap_.right = !isPolePresent;
            break;
        case Lane::Right:
            this->poleMap_.left = !isPolePresent;
            this->poleMap_.center = isPolePresent;
            break;
        default:
            break;
    }

    if (isPolePresent) {
        this->orientation_ =
            isCheckingLeft ? Orientation::Left : Orientation::Right;
    }

    resetDistanceValues();
    this->isCheckingInPosition_ = false;

    return true;
}

void MazeChallengeHandler::handleCurrentLaneIsFreeLane(
    LineFollowerConfiguration& configuration) {

    switch (this->orientation_) {
        case Orientation::Forward:
            configuration.state = LineFollowerState::FORWARD;
            break;
        case Orientation::Left:
            configuration.state = LineFollowerState::TURNING_RIGHT;
            break;
        case Orientation::Right:
            configuration.state = LineFollowerState::TURNING_LEFT;
            break;
    }

    configuration.isEventOnThree = true;
    configuration.isAutomatic = true;
    configuration.isTurnInPlace = false;

    this->isIntermediateStep_ = !this->isIntermediateStep_;
    if (!this->isIntermediateStep_) {
        if (this->currentStage == Stage::Stage1)
            this->currentStage = Stage::Stage2;
        else if (this->currentStage == Stage::Stage2)
            this->currentStage = Stage::End;

        this->isIntermediateStep_ = false;
        this->poleMap_.left = false;
        this->poleMap_.center = false;
        this->poleMap_.right = false;
    }
}

void MazeChallengeHandler::handleNextLaneIsFreeLane(
    LineFollowerConfiguration& configuration) {
    const Lane freeLane = this->getFreeLane();

    configuration.isAutomatic = false;
    configuration.isTurnInPlace = false;
    configuration.isSkippingStartingLine = true;

    const bool isOnRight = freeLane == Lane::Right || this->lane_ == Lane::Left;

    if (this->orientation_
        != (isOnRight ? Orientation::Right : Orientation::Left)) {
        configuration.state = isOnRight ? LineFollowerState::TURNING_RIGHT
                                        : LineFollowerState::TURNING_LEFT;
    } else {
        configuration.state = LineFollowerState::FORWARD;
        this->lane_ = freeLane;
    }
}

void MazeChallengeHandler::handleFarLaneIsFreeLane(
    LineFollowerConfiguration& configuration) {
    this->lane_ = Lane::Center;

    configuration.isAutomatic = true;
    configuration.isTurnInPlace = false;
    configuration.isSkippingStartingLine = true;

    configuration.state = this->lane_ == Lane::Left
                              ? LineFollowerState::TURNING_RIGHT
                              : LineFollowerState::TURNING_LEFT;
}

void MazeChallengeHandler::updateOrientation(LineFollowerState nextState) {
    if (nextState == LineFollowerState::TURNING_LEFT
        || nextState == LineFollowerState::TURNING_RIGHT) {
        switch (this->orientation_) {
            case Orientation::Forward:
                this->orientation_ =
                    nextState == LineFollowerState::TURNING_RIGHT
                        ? Orientation::Right
                        : Orientation::Left;
                break;
            case Orientation::Left:
            case Orientation::Right:
                this->orientation_ = Orientation::Forward;
                break;
        }
    }
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
    configuration.isSkippingStartingLine = false;

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
