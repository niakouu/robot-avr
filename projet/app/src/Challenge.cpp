#include "Challenge.h"

#include <string.h>

Challenge Challenge::challenge_{};

Challenge::Challenge() noexcept
    : challengeStateTracker_(0),
      lineFollower_(Robot::get().getMovementManager(),
                    Robot::get().getLineSensor(), SPEED),
      stateHolder_{State::INITIALIZATION} {}

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
            break;
        case State::HOUSE_CHALLENGE:
            this->stateHolder_.handler.house.update(deltaTimeMs, *this);
            break;
        case State::MAZE_CHALLENGE:
            this->stateHolder_.handler.maze.update(deltaTimeMs, *this);
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

    this->lineFollower_.update(deltaTimeMs);
}

void Challenge::setState(State state) {
    if (this->stateHolder_.state == State::FORK_CHALLENGE
        || this->stateHolder_.state == State::HOUSE_CHALLENGE
        || this->stateHolder_.state == State::MAZE_CHALLENGE)
        ++this->challengeStateTracker_;

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

void Challenge::parkHandler() {}

void Challenge::finishHandler() {
    const float frequency = 2.0F;
    const float period = (1.0F / frequency) * 1000.0F;

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
        case State::MAZE_CHALLENGE:
            this->handler.maze = MazeChallengeHandler();
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
        case Challenge::State::MAZE_CHALLENGE:
            handler.maze.~MazeChallengeHandler();
        default:
            break;
    }
}
