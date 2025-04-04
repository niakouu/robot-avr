#include "Challenge.h"

Challenge::Challenge() noexcept
    : currentState_(State::INITIALIZATION), challengeStateTracker_(0),
      robot_(Robot::get()),
      lineFollower_(robot_.getMovementManager(), robot_.getLineSensor(), SPEED) {}

Challenge& Challenge::get() {
    return Challenge::challenge_;
}

void Challenge::update(uint16_t deltaTimeMs) {
    switch (currentState_) {
        case State::INITIALIZATION:
            initiazliationHandler();
            break;
        case State::FOLLOW_LINE:
            followLineHandler();
            break;
        case State::LOCATE:
            locateHandler();
            break;
        case State::FORK_CHALLENGE:
            forkChallengeHandler();
            challengeStateTracker_++;
            break;
        case State::HOUSE_CHALLENGE:
            houseChallengeHandler();
            challengeStateTracker_++;
            break;
        case State::MAZE_CHALLENGE:
            mazeChallengeHandler();
            challengeStateTracker_++;
            break;
        case State::PARK:
            parkHandler();
            break;
        case State::FINISH:
            finishHandler();
            break;
        default:
            break;
    }
}

void Challenge::initiazliationHandler() {}

void Challenge::followLineHandler() {}

void Challenge::locateHandler() {}

void Challenge::forkChallengeHandler() {}

void Challenge::houseChallengeHandler() {}

void Challenge::mazeChallengeHandler() {}

void Challenge::parkHandler() {}

void Challenge::finishHandler() {
    const uint8_t frequence = 2;
    const uint8_t period = (1 / frequence) * 1000;

    Robot::get().getMovementManager().stop();

    Robot::get().getBidirectionalLed().setColor(BidirectionalLed::Color::RED);
    Board::get().getWatchdogTimer().sleep(period,
                                          WatchdogTimer::SleepMode::IDLE);
    Robot::get().getBidirectionalLed().setColor(BidirectionalLed::Color::GREEN);
    Board::get().getWatchdogTimer().sleep(period,
                                          WatchdogTimer::SleepMode::IDLE);
}