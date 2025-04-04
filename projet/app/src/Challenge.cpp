#include "Challenge.h"

#include <string.h>

Challenge Challenge::challenge_{};

Challenge::Challenge() noexcept
    : challengeStateTracker_(0),
      lineFollower_(Robot::get().getMovementManager(),
                    Robot::get().getLineSensor(), SPEED),
      stateHolder_(State::INITIALIZATION) {}

Challenge& Challenge::get() {
    return Challenge::challenge_;
}

void Challenge::update(uint16_t deltaTimeMs) {
    switch (stateHolder_.state) {
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

void Challenge::setState(State state) {
    this->stateHolder_.~StateHolder();
    new (&this->stateHolder_) StateHolder{state};
}

LineFollower<uint8_t, TimerPrescalerSynchronous>& Challenge::getLineFollower() {
    return this->lineFollower_;
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

Challenge::StateHolder::StateHolder(State state) : state(state) {
    switch (state) {
        case State::HOUSE_CHALLENGE:
            this->handler.house = HouseChallengeHandler();
            break;
        default:
            this->handler.none = 0;
    }
}

Challenge::StateHolder::~StateHolder() {
    switch (state) {
        case Challenge::State::HOUSE_CHALLENGE:
            handler.house.~HouseChallengeHandler();
            break;
        default:
            break;
    }
}
