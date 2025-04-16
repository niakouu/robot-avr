#include "HouseChallengeHandler.h"

#include "Challenge.h"

constexpr uint16_t MS_IN_S = 1000U;

HouseChallengeHandler::HouseChallengeHandler()
    : point_(Point::E_INITIAL), isPolePresent_(false), isDone_(false),
      minimumPoleDistance_(UINT8_MAX), totalReadings_(POLE_READING_COUNT) {}

void HouseChallengeHandler::flashLed(BidirectionalLed::Color color) {
    constexpr uint16_t MS_IN_S = 1000U;
    constexpr uint16_t CYCLE_TIME_MS = (MS_IN_S / FLASH_FREQ) / 2;

    for (uint8_t i = 0; i < FLASH_DURATION_MS / CYCLE_TIME_MS; ++i) {
        Board::get().getWatchdogTimer().sleep(CYCLE_TIME_MS,
                                              WatchdogTimer::SleepMode::IDLE);

        Robot::get().getBidirectionalLed().setColor(
            i % 2 == 0 ? color : BidirectionalLed::Color::OFF);
    }
}

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
                LineFollowerConfiguration::TURN_WHEEL_ADJUST_TIME_LONG_MS;
            configuration.isAutomatic = false;
            configuration.isAlignAfterTurn = true;
            configuration.isSkippingStartingLine = false;
            this->point_ = Point::G_CHECK;
            break;
        case Point::G_CHECK:
            if (this->totalReadings_ != 0) {
                --this->totalReadings_;

                const uint8_t reading =
                    Robot::get().getDistanceSensor().getDistanceCm();

                if (reading < this->minimumPoleDistance_)
                    this->minimumPoleDistance_ = reading;

                return;
            }

            if (this->minimumPoleDistance_ < POLE_DISTANCE) {
                flashLed(BidirectionalLed::Color::RED);

                configuration.state = LineFollowerState::TURNING_RIGHT;
                configuration.adjustTimeMs = 0;
                this->point_ = Point::I_FROM_G;
            } else {
                flashLed(BidirectionalLed::Color::GREEN);

                configuration.state = LineFollowerState::FORWARD;
                this->point_ = Point::H;
            }

            break;
        case Point::H:
            configuration.state = LineFollowerState::TURNING_RIGHT;
            this->point_ = Point::I_FROM_H;
            break;
        case Point::I_FROM_H:
            configuration.state = LineFollowerState::TURNING_RIGHT;
            configuration.isEventOnThree = false;
            configuration.adjustTimeMs =
                LineFollowerConfiguration::TURN_WHEEL_ADJUST_TIME_LONG_MS;
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
