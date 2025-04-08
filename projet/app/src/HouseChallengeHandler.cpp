#include "HouseChallengeHandler.h"

#include "Challenge.h"

HouseChallengeHandler::HouseChallengeHandler()
    : point_(Point::E_INITIAL), isPolePresent_(false),
      sweepTimeLeftMs_(SWEEP_TIME_MS), averagePoleDistance_(0),
      totalReadings_(POLE_READING_COUNT) {}

HouseChallengeHandler::~HouseChallengeHandler() {}

void HouseChallengeHandler::update(uint16_t deltaTimeMs, Challenge& challenge) {
    LineFollower<uint8_t, TimerPrescalerSynchronous>& lineFollower =
        challenge.getLineFollower();

    if (!lineFollower.isLost()) {
        return;
    }

    LineFollowerConfiguration configuration{.isAutomatic = true,
                                            .isEventOnThree = true,
                                            .isTurnInPlace = false,
                                            .isSkippingLine = true};

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
            configuration.isAutomatic = false;
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

            printf("pole? %d\n", this->averagePoleDistance_ < 15);
            if (this->averagePoleDistance_ < 15) {
                configuration.state = LineFollowerState::TURNING_RIGHT;
                configuration.isTurnInPlace = true;
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
            Robot::get().getMovementManager().kickstartMotors(KickstartDirection::FORWARD, KickstartDirection::FORWARD);
            configuration.state = LineFollowerState::TURNING_RIGHT;
            configuration.isEventOnThree = false;
            configuration.isTurnInPlace = true;
            configuration.isSkippingLine = false;
            this->point_ = Point::E_FINAL;
            break;
        case Point::I_FROM_G:
            configuration.state = LineFollowerState::TURNING_RIGHT;
            configuration.isEventOnThree = false;
            this->point_ = Point::E_FINAL;
            break;
        case Point::E_FINAL:
            configuration.state = LineFollowerState::TURNING_RIGHT;
            this->point_ = Point::F_FINAL;
            break;
        case Point::F_FINAL:
            configuration.state = LineFollowerState::FORWARD;
            challenge.setState(Challenge::State::FINISH);
            break;
    }

    lineFollower.start(configuration);
}
