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

    printf("got delta, p: %d\n", static_cast<uint8_t>(point_));

    LineFollowerConfiguration configuration{.isAutomatic = true,
                                            .isEventOnThree = true,
                                            .isTurnInPlace = false,
                                            .isTurnBlindAtStart = true};

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

            if (this->averagePoleDistance_ < 15) {
                configuration.state = LineFollowerState::TURNING_RIGHT;
                configuration.isTurnInPlace = true;
                configuration.isTurnBlindAtStart = false;
                this->point_ = Point::I;
            } else {
                configuration.state = LineFollowerState::FORWARD;
                this->point_ = Point::H;
            }
            break;
        case Point::H:
            configuration.state = LineFollowerState::FORWARD;
            this->point_ = Point::I;
            break;
        case Point::I:
            configuration.state = LineFollowerState::TURNING_RIGHT;
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

void HouseChallengeHandler::sweepForPole(uint16_t deltaTimeMs) {
    Robot& robot = Robot::get();

    // constexpr uint16_t TIME_OFFSET_MS = SWEEP_TIME_MS / 4;

    // if (this->sweepTimeLeftMs_ == 0)
    //     robot.getMovementManager().moveForward(0.5F);
    // else if (this->sw)

    // if (this->sweepTimeLeftMs_ > SWEEP_TIME_MS - TIME_OFFSET_MS
    //     || this->sweepTimeLeftMs_ < TIME_OFFSET_MS)
    //     robot.getMovementManager().moveLeft(Challenge::SPEED, 1.0F);
    // else
    //     robot.getMovementManager().moveRight(Challenge::SPEED, 1.0F);

    // this->isPolePresent_ |= robot.getDistanceSensor().getDistanceCm() <= 20;

    // this->sweepTimeLeftMs_ =
    //     cappingSubtract(this->sweepTimeLeftMs_, deltaTimeMs);
}
