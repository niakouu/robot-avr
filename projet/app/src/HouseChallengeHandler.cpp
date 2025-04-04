#include "HouseChallengeHandler.h"

#include "Challenge.h"

HouseChallengeHandler::HouseChallengeHandler()
    : point_(Point::E_INITIAL), isPolePresent_(false), sweepTimeLeftMs_(SWEEP_TIME_MS) {}

HouseChallengeHandler::~HouseChallengeHandler() {}

void HouseChallengeHandler::update(uint16_t deltaTimeMs, Challenge& challenge) {
    LineFollower<uint8_t, TimerPrescalerSynchronous>& lineFollower =
        challenge.getLineFollower();

    if (!lineFollower.isEvent()) {
        return;
    }

    switch (this->point_) {
        case Point::E_INITIAL:
            lineFollower.start();
            break;
        case Point::F:
            lineFollower.start(
                LineFollower<uint8_t,
                             TimerPrescalerSynchronous>::State::TURNING_RIGHT);
            break;
        case Point::G:
            
            break;
        case Point::H:
            break;
        case Point::I:
            break;
    }
}

void HouseChallengeHandler::sweepForPole(uint16_t deltaTimeMs) {
    Robot& robot = Robot::get();

    constexpr uint16_t TIME_OFFSET_MS = SWEEP_TIME_MS / 4;

    if (this->sweepTimeLeftMs_ < deltaTimeMs)
        robot.getMovementManager().stop();
    if (this->sweepTimeLeftMs_ > SWEEP_TIME_MS - TIME_OFFSET_MS || this->sweepTimeLeftMs_ < TIME_OFFSET_MS)
        robot.getMovementManager().moveLeft(Challenge::SPEED, 0.0F);
    else
        robot.getMovementManager().moveRight(Challenge::SPEED, 0.0F);
}
