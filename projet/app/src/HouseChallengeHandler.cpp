#include "HouseChallengeHandler.h"

#include "Challenge.h"

HouseChallengeHandler::HouseChallengeHandler()
    : point_(Point::E_INITIAL), isPolePresent_(false), isDone_(false),
      sweepTimeLeftMs_(SWEEP_TIME_MS), averagePoleDistance_(0),
      totalReadings_(POLE_READING_COUNT) {}

void HouseChallengeHandler::update(uint16_t deltaTimeMs, Challenge& challenge) {
    LineFollower<uint8_t, TimerPrescalerSynchronous>& lineFollower =
        challenge.getLineFollower();

    if (!lineFollower.isLost())
        return;

    LineFollowerConfiguration configuration{
        .isAutomatic = true,
        .isAlignAfterTurn = false,
        .isEventOnThree = true,
        .isSkippingStartingLine = true,
        .adjustTimeMs =
            LineFollowerConfiguration::TURN_WHEEL_ADJUST_TIME_LONG_MS};

    switch (this->point_) {
        case Point::E_INITIAL:
            configuration.state = LineFollowerState::FORWARD;
            this->point_ = Point::F;
            break;
        case Point::F:
            configuration.state = LineFollowerState::TURNING_RIGHT;
            this->point_ = Point::G;
            break;
        case Point::G:
            configuration.state = LineFollowerState::TURNING_RIGHT;
            configuration.adjustTimeMs =
                LineFollowerConfiguration::TURN_WHEEL_ADJUST_TIME_SHORT_MS;
            configuration.isAutomatic = false;
            configuration.isAlignAfterTurn = true;
            configuration.isSkippingStartingLine = false;
            this->point_ = Point::G_CHECK;
            break;
        case Point::G_CHECK:
            if (this->totalReadings_ != 0) {
                --this->totalReadings_;
                this->averagePoleDistance_ +=
                    Robot::get().getDistanceSensor().getDistanceCm();
                return;
            }

            this->averagePoleDistance_ /= POLE_READING_COUNT;

            if (this->averagePoleDistance_ < 15) {
                configuration.state = LineFollowerState::TURNING_RIGHT;
                configuration.adjustTimeMs = 0;
                this->point_ = Point::I_FROM_G;
            } else {
                configuration.state = LineFollowerState::FORWARD;
                this->point_ = Point::H;
            }
            break;
        case Point::H:
            configuration.state = LineFollowerState::TURNING_RIGHT;
            this->point_ = Point::I_FROM_H;
            break;
        case Point::I_FROM_H:
            Robot::get().getMovementManager().kickstartMotors(
                KickstartDirection::FORWARD, KickstartDirection::FORWARD);
            configuration.state = LineFollowerState::TURNING_RIGHT;
            configuration.isEventOnThree = false;
            configuration.adjustTimeMs = 0;
            configuration.isSkippingStartingLine = false;
            this->point_ = Point::E_FINAL;
            break;
        case Point::I_FROM_G:
            configuration.state = LineFollowerState::TURNING_RIGHT;
            configuration.isEventOnThree = false;
            configuration.adjustTimeMs =
                LineFollowerConfiguration::TURN_WHEEL_ADJUST_TIME_SHORT_MS;
            this->point_ = Point::E_FINAL;
            break;
        case Point::E_FINAL:
            configuration.state = LineFollowerState::TURNING_RIGHT;
            this->point_ = Point::F_FINAL;
            break;
        case Point::F_FINAL:
            this->isDone_ = true;
            configuration.state = LineFollowerState::STOP;
            break;
    }

    lineFollower.start(configuration);
}

bool HouseChallengeHandler::isDone() {
    return this->isDone_;
}
