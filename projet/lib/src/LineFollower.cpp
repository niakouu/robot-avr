#include "LineFollower.h"

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
    this->setState(LineFollower::State::STOP);
}

template <typename T, typename U>
void LineFollower<T, U>::start() {
    this->movementManager_.kickstartMotors(KickstartDirection::FORWARD, KickstartDirection::FORWARD);
    this->setState(LineFollower::State::FORWARD);
}

template <typename T, typename U>
void LineFollower<T, U>::update() {
    LineSensor::Readings readings = this->lineSensor_.getReadings();

    switch (this->currentState_) {
        case LineFollower::State::FORWARD:
            this->forwardHandler(readings);
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

    this->switchedState_ = false;
}

template <typename T, typename U>
bool LineFollower<T, U>::isEvent() const {
    return this->currentState_ == State::LOST;
}

template <typename T, typename U>
void LineFollower<T, U>::setState(State state) {
    this->currentState_ = state;
    this->switchedState_ = true;
}

template <typename T, typename U>
void LineFollower<T, U>::forwardHandler(LineSensor::Readings readings) {
    const int8_t average = readings.getAverage();
    const uint8_t darkLines = readings.getDarkLineCount();
    if (darkLines == 0 || darkLines > 2) {
        if (average < 0)
            this->currentState_ = State::TURNING_LEFT;
        else if (average > 0)
            this->currentState_ = State::TURNING_RIGHT;
        else
            this->currentState_ = State::LOST;
        return;
    }

    const float leftOffset = static_cast<float>(average) * (this->speed_ / 4.0F);
    this->movementManager_.move(true, clamp(this->speed_ + leftOffset, 0.0F, this->speed_),
                                true, clamp(this->speed_ - leftOffset, 0.0F, this->speed_));
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
