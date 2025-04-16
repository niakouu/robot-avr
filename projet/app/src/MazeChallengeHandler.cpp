#include "MazeChallengeHandler.h"

#include "Challenge.h"
#include "Robot.h"

MazeChallengeHandler::MazeChallengeHandler()
    : rotateTimeLeftMs_(TURN_TIME_MS), counter_(0),
      totalReadings_(POLE_READING_COUNT), minimumPoleDistance_(UINT8_MAX),
      finishedCalculatingPole_(false), poleMap_{false, false, false},
      currentStage_(Stage::STAGE_1), orientation_(Orientation::FORWARD),
      lane_(Lane::CENTER), isIntermediateStep_(false),
      isCheckingInPosition_(false), isDone_(false), sweepTimeLeft_(0) {}

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

    bool isManual = false;

    if (this->currentStage_ != Stage::END) {
        const Lane freeLane = this->getFreeLane();

        if (this->lane_ == freeLane) {
            this->handleCurrentLaneIsFreeLane(configuration, deltaTimeMs);
        } else if (
            // We move one lane over
            // clang-format off
            (this->lane_ == Lane::CENTER && (
                freeLane == Lane::LEFT || freeLane == Lane::RIGHT)) ||
            (freeLane == Lane::CENTER && (
                this->lane_ == Lane::LEFT || this->lane_ == Lane::RIGHT))
            // clang-format on
        ) {
            this->handleNextLaneIsFreeLane(configuration);
        } else if (
            // Far lane is free lane
            (this->lane_ == Lane::LEFT && freeLane == Lane::RIGHT)
            || (this->lane_ == Lane::RIGHT && freeLane == Lane::LEFT)) {
            this->handleFarLaneIsFreeLane(configuration);
        } else if (freeLane == Lane::INVALID) {
            bool finishedDetecting = true;
            const PoleMap previousPoleMap = this->poleMap_;
            if (this->orientation_ == Orientation::FORWARD)
                finishedDetecting =
                    handleDetection(configuration, deltaTimeMs, isManual);

            if (finishedDetecting && this->getFreeLane() == Lane::INVALID)
                this->handleCheckNextLane(configuration, previousPoleMap);
        }
    } else {
        configuration.isAutomatic = false;
        configuration.state = this->orientation_ == Orientation::FORWARD
                                  ? LineFollowerState::TURNING_RIGHT
                                  : LineFollowerState::FORWARD;

        if (this->orientation_ == Orientation::RIGHT) {
            switch (this->lane_) {
                case Lane::LEFT:
                    this->lane_ = Lane::CENTER;
                    break;
                case Lane::CENTER:
                    this->lane_ = Lane::RIGHT;
                    break;
                case Lane::RIGHT:
                default:
                    configuration.state = LineFollowerState::STOP;
                    this->isDone_ = true;
                    break;
            }
        }
    }

    if (!isManual) {
        this->updateOrientation(configuration.state);
        lineFollower.start(configuration);
    }
}

bool MazeChallengeHandler::isDone() {
    return this->isDone_;
}

MazeChallengeHandler::Lane MazeChallengeHandler::getFreeLane() const {
    if (!this->poleMap_.left && this->poleMap_.center && this->poleMap_.right)
        return Lane::LEFT;

    if (this->poleMap_.left && !this->poleMap_.center && this->poleMap_.right)
        return Lane::CENTER;

    if (this->poleMap_.left && this->poleMap_.center && !this->poleMap_.right)
        return Lane::RIGHT;

    return Lane::INVALID;
}

bool MazeChallengeHandler::handleDetection(
    LineFollowerConfiguration& configuration, uint16_t deltatimeMs,
    bool& isManual) {
    isManual = false;
    const LineSensor::Readings readings =
        Robot::get().getLineSensor().getReadings();

    MovementManager<uint8_t, TimerPrescalerSynchronous>& movementManager =
        Robot::get().getMovementManager();

    if (readings.getDarkLineCount() > 3) {
        configuration.state = LineFollowerState::ALIGN;
        configuration.isAutomatic = false;
        return false;
    }

    isManual = true;
    if (this->sweepTimeLeft_ == 0) {
        movementManager.stop();
        this->sweepTimeLeft_ = SWEEP_TIME_MS;
    } else {
        this->sweepTimeLeft_ =
            ::saturatingSubtract(this->sweepTimeLeft_, deltatimeMs);

        if (readings.getAverage() < 0) {
            movementManager.moveLeft(SWEEP_SPEED, 1.0F);
        } else {
            movementManager.moveRight(SWEEP_SPEED, 1.0F);
        }
    }

    if (!this->finishedCalculatingPole_) {
        this->calculatePoleDistance();
        return false;
    }

    isManual = false;
    const bool isPolePresent = this->isPolePresent(DISTANCE_TO_CENTER);

    switch (this->lane_) {
        case Lane::LEFT:
            this->poleMap_.left |= isPolePresent;
            this->poleMap_.center |= !isPolePresent;
            this->poleMap_.right |= !isPolePresent;
            break;
        case Lane::CENTER:
            this->poleMap_.left |= !isPolePresent;
            this->poleMap_.center |= isPolePresent;
            this->poleMap_.right |= !isPolePresent;
            break;
        case Lane::RIGHT:
            this->poleMap_.left |= !isPolePresent;
            this->poleMap_.center |= !isPolePresent;
            this->poleMap_.right |= isPolePresent;
            break;
        default:
            break;
    }

    this->resetDistanceValues();
    if (isPolePresent) {
        if (this->lane_ == Lane::RIGHT
            || (this->lane_ == Lane::CENTER && !poleMap_.left)) {
            movementManager.moveLeft(SWEEP_SPEED, 1.0F);
        } else {
            movementManager.moveRight(SWEEP_SPEED, 1.0F);
        }

        Board::get().getWatchdogTimer().sleep(SWEEP_TIME_MS, WatchdogTimer::SleepMode::IDLE);
    }

    movementManager.stop();

    return true;
}

void MazeChallengeHandler::handleCheckNextLane(
    LineFollowerConfiguration& configuration, const PoleMap& previousPoleMap) {
    configuration.isAutomatic = false;
    configuration.adjustTimeMs =
        LineFollowerConfiguration::TURN_WHEEL_ADJUST_TIME_LONG_MS;
    configuration.isAlignAfterTurn = true;

    if (previousPoleMap.left || previousPoleMap.center
        || previousPoleMap.right) {
        configuration.adjustTimeMs =
            this->orientation_ == Orientation::FORWARD
                ? 0
                : LineFollowerConfiguration::TURN_WHEEL_ADJUST_TIME_LONG_MS;
    }

    configuration.isSkippingStartingLine = true;

    const bool shouldCheckRight = this->lane_ == Lane::LEFT;

    if (this->orientation_ == Orientation::FORWARD) {
        configuration.isAutomatic = true;
        configuration.state = shouldCheckRight
                                  ? LineFollowerState::TURNING_RIGHT
                                  : LineFollowerState::TURNING_LEFT;
    } else {
        configuration.state = shouldCheckRight
                                  ? LineFollowerState::TURNING_LEFT
                                  : LineFollowerState::TURNING_RIGHT;

        switch (this->lane_) {
            case Lane::LEFT:
            case Lane::RIGHT:
                this->lane_ = Lane::CENTER;
                break;
            case Lane::CENTER:
                this->lane_ = shouldCheckRight ? Lane::RIGHT : Lane::LEFT;
                break;
            default:
                break;
        }
    }
}

void MazeChallengeHandler::handleCurrentLaneIsFreeLane(
    LineFollowerConfiguration& configuration, uint16_t deltaTimeMs) {

    if (this->orientation_ == Orientation::FORWARD
        && !this->isIntermediateStep_) {
        if (!this->handleDetectionDance(deltaTimeMs))
            return;
    }

    switch (this->orientation_) {
        case Orientation::FORWARD:
            configuration.state = LineFollowerState::FORWARD;
            break;
        case Orientation::LEFT:
            configuration.state = LineFollowerState::TURNING_RIGHT;
            break;
        case Orientation::RIGHT:
            configuration.state = LineFollowerState::TURNING_LEFT;
            break;
    }

    configuration.isEventOnThree = true;
    configuration.isAutomatic = false;
    configuration.adjustTimeMs =
        LineFollowerConfiguration::TURN_WHEEL_ADJUST_TIME_LONG_MS;

    if (this->orientation_ != Orientation::FORWARD)
        return;

    this->isIntermediateStep_ = !this->isIntermediateStep_;
    if (!this->isIntermediateStep_) {
        if (this->currentStage_ == Stage::STAGE_1)
            this->currentStage_ = Stage::STAGE_2;
        else if (this->currentStage_ == Stage::STAGE_2)
            this->currentStage_ = Stage::END;

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
    configuration.isAlignAfterTurn = true;
    configuration.adjustTimeMs =
        LineFollowerConfiguration::TURN_WHEEL_ADJUST_TIME_LONG_MS;
    if (this->orientation_ == Orientation::FORWARD)
        configuration.adjustTimeMs = 0;

    configuration.isSkippingStartingLine = true;
    configuration.isEventOnThree =
        this->currentStage_ == Stage::STAGE_1 && this->lane_ == Lane::CENTER;

    const bool isOnRight = freeLane == Lane::RIGHT || this->lane_ == Lane::LEFT;

    if (this->orientation_
        != (isOnRight ? Orientation::RIGHT : Orientation::LEFT)) {
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

    configuration.state = this->lane_ == Lane::LEFT
                              ? LineFollowerState::TURNING_RIGHT
                              : LineFollowerState::TURNING_LEFT;

    this->lane_ = Lane::CENTER;
}

void MazeChallengeHandler::updateOrientation(LineFollowerState nextState) {
    if (nextState == LineFollowerState::TURNING_LEFT
        || nextState == LineFollowerState::TURNING_RIGHT) {
        switch (this->orientation_) {
            case Orientation::FORWARD:
                this->orientation_ =
                    nextState == LineFollowerState::TURNING_RIGHT
                        ? Orientation::RIGHT
                        : Orientation::LEFT;
                break;
            case Orientation::LEFT:
            case Orientation::RIGHT:
                this->orientation_ = Orientation::FORWARD;
                break;
        }
    }
}

bool MazeChallengeHandler::isPolePresent(uint8_t distance) const {
    return this->minimumPoleDistance_ <= distance;
}

void MazeChallengeHandler::calculatePoleDistance() {
    if (this->totalReadings_ != 0) {
        --this->totalReadings_;

        const uint8_t reading =
            Robot::get().getDistanceSensor().getDistanceCm();

        if (reading < this->minimumPoleDistance_)
            this->minimumPoleDistance_ = reading;

        return;
    }

    this->finishedCalculatingPole_ = true;
}

void MazeChallengeHandler::resetDistanceValues() {
    this->minimumPoleDistance_ = UINT8_MAX;
    this->totalReadings_ = POLE_READING_COUNT;
    this->finishedCalculatingPole_ = false;
}

bool MazeChallengeHandler::handleDetectionDance(uint16_t deltaTimeMs) {
    constexpr uint16_t MS_IN_S = 1000U;
    constexpr uint16_t CYCLE_TIME_MS = (MS_IN_S / FLASH_FREQ) / 2;

    for (uint8_t i = 0; i < FLASH_DURATION_MS / CYCLE_TIME_MS; ++i) {
        Board::get().getWatchdogTimer().sleep(CYCLE_TIME_MS,
                                              WatchdogTimer::SleepMode::IDLE);

        Robot::get().getBidirectionalLed().setColor(
            i % 2 == 0 ? BidirectionalLed::Color::GREEN
                       : BidirectionalLed::Color::OFF);
    }

    return true;
}
