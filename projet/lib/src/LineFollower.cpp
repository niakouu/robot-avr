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
    : movementManager_(movementManager), lineSensor_(lineSensor),
      configuration_{.state = LineFollowerState::LOST}, switchedState_(true),
      lastReadings_(), speed_(speed), lastError_(0), integralComponent_(0),
      adjustTimeLeft_(0), alignTimeLeft_(0), isExitingLine_(false),
      wasLostAndIsSkippingError_(false), hasFoundGuide_(false),
      alignAttemptsLeft_(0) {}

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
    if (this->configuration_.state == LineFollowerState::LOST
        && configuration.state != LineFollowerState::LOST) {
        this->wasLostAndIsSkippingError_ = true;
    }

    this->configuration_ = configuration;

    this->switchedState_ = true;
}

template <typename T, typename U>
void LineFollower<T, U>::update(uint16_t deltaTimeMs) {
    LineSensor::Readings readings = this->lineSensor_.getReadings();

    LineFollowerState lastState = this->configuration_.state;
    switch (this->configuration_.state) {
        case LineFollowerState::FORWARD:
            this->forwardHandler(readings);
            break;
        case LineFollowerState::ALIGN:
            this->alignHandler(readings, deltaTimeMs);
            break;
        case LineFollowerState::TURNING_LEFT:
        case LineFollowerState::TURNING_RIGHT:
            this->turningHandler(readings, deltaTimeMs);
            break;
        case LineFollowerState::LOST:
            this->lostHandler();
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
void LineFollower<T, U>::forwardHandler(LineSensor::Readings readings) {

    if (this->switchedState_) {
        this->integralComponent_ = 0;
        this->lastError_ = 0;
    }

    const int8_t average = readings.getAverage();
    const uint8_t darkLines = readings.getDarkLineCount();
    if (darkLines == 0 || darkLines >= 4 || !readings.isSinglePath()
        || (this->configuration_.isEventOnThree && darkLines == 3)) {
        if (!this->wasLostAndIsSkippingError_) {
            this->configuration_.state = LineFollowerState::LOST;
            this->movementManager_.stop();
            return;
        }
    } else {
        this->wasLostAndIsSkippingError_ = false;
    }

    const int8_t error = -average;
    const int8_t deltaError = error - this->lastError_;
    this->lastError_ = error;
    this->integralComponent_ += error;

    float rightOffset =
        (static_cast<float>(error) * PID_KP)
        + (static_cast<float>(this->integralComponent_) * PID_KI)
        + (static_cast<float>(deltaError * PID_KD));

    this->movementManager_.move(
        true, clamp(this->speed_ - rightOffset, 0.0F, this->speed_), true,
        clamp(this->speed_ + rightOffset, 0.0F, this->speed_));
}

template <typename T, typename U>
void LineFollower<T, U>::alignHandler(LineSensor::Readings readings,
                                      uint16_t deltaTimeMs) {
    if (this->switchedState_) {
        this->alignTimeLeft_ = 0;
        this->alignAttemptsLeft_ = ALIGN_MAX_ATTEMPTS;
    }

    if (readings.getDarkLineCount() > 2) {
        this->movementManager_.moveForward(this->speed_);
        return;
    }

    if (readings.getDarkLineCount() == 0) {
        this->movementManager_.stop();
        this->configuration_.state = LineFollowerState::LOST;
        return;
    }

    if (this->alignTimeLeft_ != 0) {
        this->alignTimeLeft_ =
            ::saturatingSubtract(this->alignTimeLeft_, deltaTimeMs);

        if (this->alignTimeLeft_ == 0) {
            this->movementManager_.stop();
        }
    } else if ((readings.isCenterDark && readings.getDarkLineCount() == 1)
               || --alignAttemptsLeft_ == 0) {
        this->configuration_.state = this->configuration_.isAutomatic
                                         ? LineFollowerState::FORWARD
                                         : LineFollowerState::LOST;
    } else {
        this->alignTimeLeft_ = ALIGN_TIME_MS;
        if (readings.getAverage() > 0) {
            this->movementManager_.moveRight(ALIGN_SPEED, 1.0F);
        } else {
            this->movementManager_.moveLeft(ALIGN_SPEED, 1.0F);
        }
    }
}

template <typename T, typename U>
void LineFollower<T, U>::lostHandler() {
    if (this->switchedState_) {
        this->movementManager_.stop();
    }
}

template <typename T, typename U>
void LineFollower<T, U>::turningHandler(LineSensor::Readings readings,
                                        uint16_t deltaTimeMs) {
    if (this->switchedState_) {
        this->hasFoundGuide_ = false;
        this->isExitingLine_ = readings.getDarkLineCount() != 0
                               && this->configuration_.isSkippingStartingLine;
        this->adjustTimeLeft_ = this->configuration_.adjustTimeMs;

        if (this->configuration_.adjustTimeMs > 0) {
            this->movementManager_.moveForward(this->speed_);
            return;
        }
    }

    if (this->adjustTimeLeft_ != 0
        || (this->switchedState_ && this->configuration_.adjustTimeMs == 0)) {
        this->adjustTimeLeft_ =
            ::saturatingSubtract(this->adjustTimeLeft_, deltaTimeMs);

        if (this->adjustTimeLeft_ == 0) {
            if (this->configuration_.state == LineFollowerState::TURNING_LEFT) {
                this->movementManager_.moveLeft(this->speed_ * TURN_SPEED_RATIO,
                                                1.0F);
            } else {
                this->movementManager_.moveRight(this->speed_
                                                     * TURN_SPEED_RATIO,
                                                 1.0F);
            }
        } else {
            if (readings.getAverage() < -2) {
                this->movementManager_.move(true, this->speed_ / 2, true, this->speed_);
            } if (readings.getAverage() >= 2) {
                this->movementManager_.move(true, this->speed_, true, this->speed_ / 2);
            } else {
                this->movementManager_.moveForward(this->speed_);
            }
        }
    } else if (this->isExitingLine_) {
        this->isExitingLine_ = readings.getDarkLineCount() != 0;
    } else if (readings.getDarkLineCount() != 0 || this->hasFoundGuide_) {
        this->movementManager_.stop();
        if (!hasFoundGuide_) {
            this->alignTimeLeft_ = TURN_ALIGN_TIME_MS;
            this->hasFoundGuide_ = true;
            return;
        }

        this->alignTimeLeft_ =
            ::saturatingSubtract(this->alignTimeLeft_, deltaTimeMs);
        if (this->alignTimeLeft_ != 0) {
            return;
        }

        if (readings.getDarkLineCount() == 0) {
            this->hasFoundGuide_ = false;

            if (this->configuration_.state == LineFollowerState::TURNING_LEFT)
                this->movementManager_.moveRight(this->speed_
                                                     * TURN_SPEED_RATIO,
                                                 1.0F);
            else
                this->movementManager_.moveLeft(this->speed_ * TURN_SPEED_RATIO,
                                                1.0F);

            return;
        }

        if (this->configuration_.isAlignAfterTurn)
            this->configuration_.state = LineFollowerState::ALIGN;
        else if (this->configuration_.isAutomatic)
            this->configuration_.state = LineFollowerState::FORWARD;
        else
            this->configuration_.state = LineFollowerState::LOST;
    }
}
