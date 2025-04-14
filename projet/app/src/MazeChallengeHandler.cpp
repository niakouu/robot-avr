#include "MazeChallengeHandler.h"

#include "Challenge.h"
#include "Robot.h"

MazeChallengeHandler::MazeChallengeHandler()
    : rotateTimeLeftMs_(TURN_TIME_MS), averagePoleDistance_(0),
      detectionDanceTimeLeftMs_(0), counter_(0),
      totalReadings_(POLE_READING_COUNT), finishedCalculatingPole_(false),
      poleMap_{false, false, false}, currentStage_(Stage::Stage1),
      orientation_(Orientation::Forward), lane_(Lane::Center),
      isIntermediateStep_(false), isCheckingInPosition_(false), isDone_(false) {
}

void MazeChallengeHandler::update(uint16_t deltaTimeMs, Challenge& challenge) {
    LineFollower<uint8_t, TimerPrescalerSynchronous>& lineFollower =
        challenge.getLineFollower();

    if (!lineFollower.isLost())
        return;

    LineFollowerConfiguration configuration{
        .state = LineFollowerState::LOST,
        .isAutomatic = true,
        .isAlignAfterTurn = false,
        .isEventOnThree = true,
        .isSkippingStartingLine = true,
        .adjustTimeMs =
            LineFollowerConfiguration::TURN_WHEEL_ADJUST_TIME_LONG_MS};

    if (this->currentStage_ != Stage::End) {
        const Lane freeLane = this->getFreeLane();
        printf("stage: %s o:%s l:%s f:%s\n", toString(this->currentStage_),
               toString(this->orientation_), toString(this->lane_),
               toString(freeLane));

        if (this->lane_ == freeLane) {
            this->handleCurrentLaneIsFreeLane(configuration, deltaTimeMs);
        } else if (
            // We move one lane over
            // clang-format off
            (this->lane_ == Lane::Center && (
                freeLane == Lane::Left || freeLane == Lane::Right)) ||
            (freeLane == Lane::Center && (
                this->lane_ == Lane::Left || this->lane_ == Lane::Right))
            // clang-format on
        ) {
            this->handleNextLaneIsFreeLane(configuration);
        } else if (
            // Far lane is free lane
            (this->lane_ == Lane::Left && freeLane == Lane::Right)
            || (this->lane_ == Lane::Right && freeLane == Lane::Left)) {
            this->handleFarLaneIsFreeLane(configuration);
        } else if (freeLane == Lane::Invalid) {
            bool finishedDetecting = true;
            const PoleMap previousPoleMap = this->poleMap_;
            if (this->orientation_ == Orientation::Forward)
                finishedDetecting = handleDetection(configuration);

            if (finishedDetecting && this->getFreeLane() == Lane::Invalid)
                this->handleCheckNextLane(configuration, previousPoleMap);
        }
    } else {
        configuration.isAutomatic = false;
        configuration.state = this->orientation_ == Orientation::Forward
                                  ? LineFollowerState::TURNING_RIGHT
                                  : LineFollowerState::FORWARD;

        if (this->orientation_ == Orientation::Right) {
            switch (this->lane_) {
                case Lane::Left:
                    this->lane_ = Lane::Center;
                    break;
                case Lane::Center:
                    this->lane_ = Lane::Right;
                    break;
                case Lane::Right:
                default:
                    configuration.state = LineFollowerState::STOP;
                    this->isDone_ = true;
                    break;
            }
        }
    }

    this->updateOrientation(configuration.state);

    printf("  -> s: %s\n", toString(configuration.state));
    lineFollower.start(configuration);
}

bool MazeChallengeHandler::isDone() {
    return this->isDone_;
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

bool MazeChallengeHandler::handleDetection(
    LineFollowerConfiguration& configuration) {
    if (Robot::get().getLineSensor().getReadings().getDarkLineCount() > 2) {
        configuration.state = LineFollowerState::ALIGN;
        configuration.isAutomatic = false;
        return false;
    }

    if (!this->finishedCalculatingPole_) {
        this->calculatePoleDistance();
        return false;
    }

    printf("distance: %d\n", this->averagePoleDistance_);
    const bool isPolePresent = this->isPolePresent(DISTANCE_TO_CENTER);

    switch (this->lane_) {
        case Lane::Left:
            this->poleMap_.left |= isPolePresent;
            this->poleMap_.center |= !isPolePresent;
            this->poleMap_.right |= !isPolePresent;
            break;
        case Lane::Center:
            this->poleMap_.left |= !isPolePresent;
            this->poleMap_.center |= isPolePresent;
            this->poleMap_.right |= !isPolePresent;
            break;
        case Lane::Right:
            this->poleMap_.left |= !isPolePresent;
            this->poleMap_.center |= !isPolePresent;
            this->poleMap_.right |= isPolePresent;
            break;
        default:
            break;
    }

    this->resetDistanceValues();

    return true;
}

void MazeChallengeHandler::handleCheckNextLane(
    LineFollowerConfiguration& configuration, const PoleMap& previousPoleMap) {
    configuration.isAutomatic = false;
    configuration.adjustTimeMs = LineFollowerConfiguration::TURN_WHEEL_ADJUST_TIME_SHORT_MS;

    if (previousPoleMap.left || previousPoleMap.center
        || previousPoleMap.right) {
        configuration.adjustTimeMs =
            this->orientation_ == Orientation::Forward
                ? 0
                : LineFollowerConfiguration::TURN_WHEEL_ADJUST_TIME_LONG_MS;
    }

    configuration.isSkippingStartingLine = true;

    const bool shouldCheckRight = this->lane_ == Lane::Left;

    if (this->orientation_ == Orientation::Forward) {
        configuration.isAutomatic = true;
        configuration.state = shouldCheckRight
                                  ? LineFollowerState::TURNING_RIGHT
                                  : LineFollowerState::TURNING_LEFT;
    } else {
        configuration.isAlignAfterTurn = true;
        configuration.state = shouldCheckRight
                                  ? LineFollowerState::TURNING_LEFT
                                  : LineFollowerState::TURNING_RIGHT;

        switch (this->lane_) {
            case Lane::Left:
            case Lane::Right:
                this->lane_ = Lane::Center;
                break;
            case Lane::Center:
                this->lane_ = shouldCheckRight ? Lane::Right : Lane::Left;
                break;
            default:
                break;
        }
    }
}

void MazeChallengeHandler::handleCurrentLaneIsFreeLane(
    LineFollowerConfiguration& configuration, uint16_t deltaTimeMs) {

    if (this->orientation_ == Orientation::Forward
        && !this->isIntermediateStep_) {
        if (!this->handleDetectionDance(deltaTimeMs))
            return;
    }

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
    configuration.adjustTimeMs =
        LineFollowerConfiguration::TURN_WHEEL_ADJUST_TIME_LONG_MS;

    this->isIntermediateStep_ = !this->isIntermediateStep_;
    if (!this->isIntermediateStep_) {
        if (this->currentStage_ == Stage::Stage1)
            this->currentStage_ = Stage::Stage2;
        else if (this->currentStage_ == Stage::Stage2)
            this->currentStage_ = Stage::End;

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
    configuration.adjustTimeMs =
        LineFollowerConfiguration::TURN_WHEEL_ADJUST_TIME_LONG_MS;
    configuration.isSkippingStartingLine = true;
    configuration.isEventOnThree =
        this->currentStage_ == Stage::Stage1 && this->lane_ == Lane::Center;

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
    configuration.isAutomatic = true;
    configuration.adjustTimeMs = 0;
    configuration.isSkippingStartingLine = true;

    configuration.state = this->lane_ == Lane::Left
                              ? LineFollowerState::TURNING_RIGHT
                              : LineFollowerState::TURNING_LEFT;

    this->lane_ = Lane::Center;
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
    this->finishedCalculatingPole_ = true;
}

void MazeChallengeHandler::resetDistanceValues() {
    this->averagePoleDistance_ = 0;
    this->totalReadings_ = POLE_READING_COUNT;
    this->finishedCalculatingPole_ = false;
}

bool MazeChallengeHandler::handleDetectionDance(uint16_t deltaTimeMs) {
    constexpr uint16_t MS_IN_S = 1000U;
    constexpr uint16_t CYCLE_TIME_MS = (MS_IN_S / FLASH_FREQ) / 2;

    // if (this->detectionDanceTimeLeftMs_ == 0) {
    //     this->detectionDanceTimeLeftMs_ = FLASH_DURATION;
    // } else {
    //     this->detectionDanceTimeLeftMs_ =
    //         cappingSubtract(this->detectionDanceTimeLeftMs_, deltaTimeMs);
    // }

    for (uint8_t i = 0; i < FLASH_DURATION_MS / CYCLE_TIME_MS; ++i) {
        Board::get().getWatchdogTimer().sleep(CYCLE_TIME_MS,
                                              WatchdogTimer::SleepMode::IDLE);

        Robot::get().getBidirectionalLed().setColor(
            i % 2 == 0 ? BidirectionalLed::Color::GREEN
                       : BidirectionalLed::Color::OFF);
    }

    return true; // this->detectionDanceTimeLeftMs_ != 0;
}
