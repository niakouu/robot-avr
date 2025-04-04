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
      currentState_(LineFollower::State::STOP), speed_(speed) {}

template <typename T, typename U>
LineFollower<T, U>::~LineFollower() {
    this->stop();
}

template <typename T, typename U>
void LineFollower<T, U>::stop() {
    this->currentState_ = LineFollower::State::STOP;
    this->switchedState_ = true;
}

template <typename T, typename U>
void LineFollower<T, U>::start() {
    this->movementManager_.kickstartMotors(KickstartDirection::FORWARD,
                                           KickstartDirection::FORWARD);
    this->currentState_ = LineFollower::State::FORWARD;
    this->switchedState_ = true;
}

template <typename T, typename U>
void LineFollower<T, U>::update(uint16_t deltaTimeMs) {
    LineSensor::Readings readings = this->lineSensor_.getReadings();

    State lastState = this->currentState_;
    switch (this->currentState_) {
        case LineFollower::State::FORWARD:
            this->forwardHandler(readings, deltaTimeMs);
            break;
        case LineFollower::State::DETECTION:
            this->detectionHandler(readings, deltaTimeMs);
            break;
        case LineFollower::State::TURNING_LEFT:
        case LineFollower::State::TURNING_RIGHT:
            this->turningHandler(readings);
            break;
        case LineFollower::State::LOST:
        case LineFollower::State::STOP:
            this->movementManager_.stop();
            break;
        default:
            break;
    }

    this->switchedState_ = lastState != this->currentState_;
}

template <typename T, typename U>
bool LineFollower<T, U>::isEvent() const {
    return this->currentState_ == State::LOST;
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
    if (darkLines == 0 || darkLines == 5) {
        this->currentState_ = State::LOST;
        return;
    }

    if (darkLines >= 3 && false) {
        this->currentState_ = State::LOST;
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

    //printf("speed: %d avg: %d\n", static_cast<uint16_t>(rightOffset * 100.0f),
     //      average);
    this->movementManager_.move(
        true, clamp(this->speed_ - rightOffset, 0.0F, this->speed_), true,
        clamp(this->speed_ + rightOffset, 0.0F, this->speed_));
}

template <typename T, typename U>
void LineFollower<T, U>::detectionHandler(LineSensor::Readings readings,
                                          uint16_t deltaTimeMs) {
    // if (this->switchedState_) {
    //     this->detectionTimeLeft = DETECTION_TIME_MS;
    //     return;
    // }

    // if (this->detectionTimeLeft < deltaTimeMs) {
    // }

    // if (this->detectionTimeLeft < DETECTION_TIME_MS / 2) {
    // }
    // this->detectionTimeLeft -= deltaTimeMs;
}

template <typename T, typename U>
void LineFollower<T, U>::turningHandler(LineSensor::Readings readings) {
    Board& board = Board::get();

    if (this->switchedState_) {
        this->movementManager_.moveForward(this->speed_);
        board.getWatchdogTimer().sleep(100, WatchdogTimer::SleepMode::IDLE);
        if (this->currentState_ == State::TURNING_LEFT)
            this->movementManager_.moveLeft(this->speed_, 0);
        else if (this->currentState_ == State::TURNING_RIGHT)
            this->movementManager_.moveRight(this->speed_, 0);
        board.getWatchdogTimer().sleep(50, WatchdogTimer::SleepMode::IDLE);
    } else {
        if (readings.isCenterDark) {
            this->currentState_ = State::FORWARD;
        }
    }
}
