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
      currentState_(LineFollowerState::STOP), speed_(speed),
      isAutomatic_(true) {}

template <typename T, typename U>
LineFollower<T, U>::~LineFollower() {
    this->stop();
}

template <typename T, typename U>
void LineFollower<T, U>::stop() {
    this->currentState_ = LineFollowerState::STOP;
    this->switchedState_ = true;
}

template <typename T, typename U>
void LineFollower<T, U>::start(LineFollowerState state, bool isAutomatic) {
    if (state == LineFollowerState::FORWARD) {
        this->movementManager_.kickstartMotors(KickstartDirection::FORWARD, KickstartDirection::FORWARD);
    }
    //printf("KICKSTART!!!\n");

    this->isAutomatic_ = isAutomatic;
    this->currentState_ = state;
    this->switchedState_ = true;
}

template <typename T, typename U>
void LineFollower<T, U>::update(uint16_t deltaTimeMs) {
    LineSensor::Readings readings = this->lineSensor_.getReadings();

    LineFollowerState lastState = this->currentState_;
    switch (this->currentState_) {
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

    this->switchedState_ = lastState != this->currentState_;
}

template <typename T, typename U>
bool LineFollower<T, U>::isLost() const {
    return this->currentState_ == LineFollowerState::LOST;
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
    if (darkLines == 0 || darkLines >= 4 || !readings.isSinglePath()) {
        //printf("darkLines: %d single: %d\n", darkLines,
        //       readings.isSinglePath());
        this->currentState_ = LineFollowerState::LOST;
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

    this->currentState_ = LineFollowerState::LOST;

    if (readings.getDarkLineCount() == 0) {
        if (this->lastReadings_.isLeftDark) {
            this->currentState_ = LineFollowerState::TURNING_LEFT;
        } else if (this->lastReadings_.isRightDark) {
            this->currentState_ = LineFollowerState::TURNING_RIGHT;
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
        this->turnIgnoreTimeLeft_ = TURN_IGNORE_TIME_MS;
        this->adjustTimeLeft_ = TURN_WHEEL_ADJUST_TIME_MS;
        this->movementManager_.moveForward(this->speed_);
        //printf("speed downgrade\n");
    } else if (this->adjustTimeLeft_ != 0) {
        this->adjustTimeLeft_ =
            cappingSubtract(this->adjustTimeLeft_, deltaTimeMs);

        if (this->adjustTimeLeft_ == 0) {
            if (this->currentState_ == LineFollowerState::TURNING_LEFT) {
                this->movementManager_.kickstartMotors(KickstartDirection::BACKWARD, KickstartDirection::FORWARD, 10);
                this->movementManager_.moveLeft(this->speed_ * 0.9F, 1.0F);
            } else {
                this->movementManager_.kickstartMotors(KickstartDirection::FORWARD, KickstartDirection::BACKWARD, 10);
                this->movementManager_.moveRight(this->speed_ * 0.9F, 1.0F);
            }
        }
    } else if (this->turnIgnoreTimeLeft_ != 0) {
        this->turnIgnoreTimeLeft_ =
            cappingSubtract(this->turnIgnoreTimeLeft_, deltaTimeMs);
    } else if (readings.isCenterDark) {
        this->currentState_ = this->isAutomatic_ ? LineFollowerState::FORWARD
                                                 : LineFollowerState::LOST;
    }
}
