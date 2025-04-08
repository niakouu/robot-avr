#include "LineFollower.h"

#include <math.h>

#include "Board.h"
#include "common.h"

template class LineFollower<uint8_t, TimerPrescalerSynchronous>;
template class LineFollower<uint16_t, TimerPrescalerSynchronous>;
template class LineFollower<uint8_t, TimerPrescalerAsynchronous>;

template <typename T, typename U>
LineFollower<T, U>::LineFollower(MovementManager<T, U>& movementManager,
                                 LineSensor& lineSensor, float speed)
    : movementManager_(movementManager), lineSensor_(lineSensor), speed_(speed),
      configuration_{.state = LineFollowerState::STOP} {}

template <typename T, typename U>
LineFollower<T, U>::~LineFollower() {
    this->stop();
}

template <typename T, typename U>
void LineFollower<T, U>::stop() {
    this->configuration_.state = LineFollowerState::STOP;
    this->switchedState_ = true;
}

template <typename T, typename U>
void LineFollower<T, U>::start(const LineFollowerConfiguration& configuration) {
    this->configuration_ = configuration;

    // if (this->configuration_.state == LineFollowerState::FORWARD) {
    //     this->movementManager_.kickstartMotors(KickstartDirection::FORWARD,
    //                                            KickstartDirection::FORWARD);
    // }

    this->switchedState_ = true;
}

template <typename T, typename U>
void LineFollower<T, U>::update(uint16_t deltaTimeMs) {
    LineSensor::Readings readings = this->lineSensor_.getReadings();

    LineFollowerState lastState = this->configuration_.state;
    switch (this->configuration_.state) {
        case LineFollowerState::FORWARD:
            this->forwardHandler(readings, deltaTimeMs);
            break;
        case LineFollowerState::DETECTION:
            this->detectionHandler(readings, deltaTimeMs);
            break;
        case LineFollowerState::TURNING_LEFT:
        case LineFollowerState::TURNING_RIGHT:
            this->turningHandler(readings, deltaTimeMs);
            break;
        case LineFollowerState::LOST:
            this->lostHandler(readings, deltaTimeMs);
            break;
        case LineFollowerState::STOP:
            this->movementManager_.stop();
            break;
        default:
            break;
    }

    this->switchedState_ = lastState != this->configuration_.state;
}

template <typename T, typename U>
bool LineFollower<T, U>::isLost() const {
    return this->configuration_.state == LineFollowerState::LOST;
}

template <typename T, typename U>
void LineFollower<T, U>::forwardHandler(LineSensor::Readings readings,
                                        uint16_t deltaTimeMs) {

    if (this->switchedState_) {
        this->integralComponent_ = 0;
        this->lastError_ = 0;
    }

    const int8_t average = readings.getAverage();
    const uint8_t darkLines = readings.getDarkLineCount();
    if (darkLines == 0 || darkLines >= 4 || !readings.isSinglePath()
        || (this->configuration_.isEventOnThree && darkLines == 3)) {
        this->configuration_.state = LineFollowerState::LOST;
        return;
    }

    const int8_t error = -average;
    const int8_t deltaError = error - this->lastError_;
    this->lastError_ = error;
    this->integralComponent_ += error;

    float rightOffset =
        (static_cast<float>(error) * PID_KP)
        + (static_cast<float>(this->integralComponent_) * PID_KI)
        + (static_cast<float>(deltaError * PID_KD));

    // printf("speed: %d avg: %d\n", static_cast<uint16_t>(rightOffset *
    // 100.0f),
    //       average);

    if (darkLines == 3) {
        if (readings.isLeftDark)
            rightOffset += 0.2;
        else if (readings.isRightDark)
            rightOffset -= 0.2;
    }

    this->movementManager_.move(
        true, clamp(this->speed_ - rightOffset, 0.0F, this->speed_), true,
        clamp(this->speed_ + rightOffset, 0.0F, this->speed_));
}

template <typename T, typename U>
void LineFollower<T, U>::detectionHandler(LineSensor::Readings readings,
                                          uint16_t deltaTimeMs) {

    this->movementManager_.stop();

    this->configuration_.state = LineFollowerState::LOST;

    if (readings.getDarkLineCount() == 0) {
        if (this->lastReadings_.isLeftDark) {
            this->configuration_.state = LineFollowerState::TURNING_LEFT;
        } else if (this->lastReadings_.isRightDark) {
            this->configuration_.state = LineFollowerState::TURNING_RIGHT;
        }
    }
}

template <typename T, typename U>
void LineFollower<T, U>::lostHandler(LineSensor::Readings readings,
                                     uint16_t deltaTimeMs) {
    if (this->switchedState_) {
        this->movementManager_.stop();
    }
}

template <typename T, typename U>
void LineFollower<T, U>::turningHandler(LineSensor::Readings readings,
                                        uint16_t deltaTimeMs) {
    if (this->switchedState_) {
        this->isExitingLine_ = readings.getDarkLineCount() != 0 && this->configuration_.isSkippingLine;
        this->adjustTimeLeft_ =
            this->configuration_.isTurnInPlace ? 0 : TURN_WHEEL_ADJUST_TIME_MS;

        if (!this->configuration_.isTurnInPlace) {
            this->movementManager_.moveForward(this->speed_);
            return;
        }
    }

    if (this->adjustTimeLeft_ != 0
        || (this->switchedState_ && this->configuration_.isTurnInPlace)) {
        this->adjustTimeLeft_ =
            cappingSubtract(this->adjustTimeLeft_, deltaTimeMs);

        if (this->adjustTimeLeft_ == 0) {
            if (this->configuration_.state == LineFollowerState::TURNING_LEFT) {
                // this->movementManager_.kickstartMotors(
                //     KickstartDirection::BACKWARD,
                //     KickstartDirection::FORWARD, 10);
                this->movementManager_.moveLeft(this->speed_, 1.0F);
            } else {
                // this->movementManager_.kickstartMotors(
                //     KickstartDirection::FORWARD,
                //     KickstartDirection::BACKWARD, 10);
                this->movementManager_.moveRight(this->speed_, 1.0F);
            }
        }
    } else if (this->isExitingLine_) {
        this->isExitingLine_ = readings.getDarkLineCount() != 0;
    } else if (readings.getDarkLineCount() != 0) {
        this->movementManager_.stop();
        printf("found guide!\n");
        if (!readings.isCenterDark) {
            if (readings.getAverage() < 4) {
                this->movementManager_.moveRight(1.0F, 0.0F);
            } else {
                this->movementManager_.moveLeft(1.0F, 0.0F);
            }
            this->movementManager_.stop();
        }
        this->configuration_.state = this->configuration_.isAutomatic
                                         ? LineFollowerState::FORWARD
                                         : LineFollowerState::LOST;
    }
}
