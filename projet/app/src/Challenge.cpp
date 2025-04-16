#include "Challenge.h"

#include <string.h>

Challenge Challenge::challenge_{};

Challenge::Challenge() noexcept
    : isTurnLeftFork_{false, false}, challengeStateTracker_(0),
      buttonCounter_(0), nextStateStep_(0),
      lineFollower_(Robot::get().getMovementManager(),
                    Robot::get().getLineSensor(), SPEED),
      switchedState_(true), previousState_(State::INITIALIZATION),
      stateHolder_{State::INITIALIZATION}, initializationSleepTimeLeft_(0) {}

Challenge& Challenge::get() {
    return Challenge::challenge_;
}

void Challenge::update(uint16_t deltaTimeMs) {
    const State lastState = stateHolder_.state;
    bool done = false;

    switch (stateHolder_.state) {
        case State::INITIALIZATION:
            this->initializationHandler();
            break;
        case State::INITIALIZATION_WAIT:
            this->initializationWaitHandler(deltaTimeMs);
            break;
        case State::NEXT_STATE:
            this->nextStateHandler();
            break;
        case State::LOCATE:
            this->locateHandler();
            break;
        case State::FORK_CHALLENGE:
            this->stateHolder_.handler.fork.update(deltaTimeMs, *this);
            done = this->stateHolder_.handler.fork.isDone();
            break;
        case State::HOUSE_CHALLENGE:
            this->stateHolder_.handler.house.update(deltaTimeMs, *this);
            done = this->stateHolder_.handler.house.isDone();
            break;
        case State::MAZE_CHALLENGE:
            this->stateHolder_.handler.maze.update(deltaTimeMs, *this);
            done = this->stateHolder_.handler.maze.isDone();
            break;
        case State::FINISH:
            finishHandler();
            break;
        default:
            break;
    }

    if (done) {
        ++this->challengeStateTracker_;

        this->setState(State::NEXT_STATE);
    }

    this->switchedState_ = this->stateHolder_.state != lastState;
    this->lineFollower_.update(deltaTimeMs);
}

void Challenge::setState(State state) {
    this->previousState_ = this->stateHolder_.state;
    this->stateHolder_.~StateHolder();
    new (&this->stateHolder_) StateHolder{state};
}

LineFollower<uint8_t, TimerPrescalerSynchronous>& Challenge::getLineFollower() {
    return this->lineFollower_;
}

bool Challenge::isTurnLeftFork(bool first) const {
    return this->isTurnLeftFork_[first ? 0 : 1];
}

void Challenge::initializationHandler() {
    Button& button = Board::get().getButton();
    Button& extraButton = Robot::get().getExtraButton();
    const BidirectionalLed& bidirectionalLed =
        Robot::get().getBidirectionalLed();

    if (!button.isEvent() && !extraButton.isEvent()) {
        return;
    }

    if (this->buttonCounter_ == 2) {
        bidirectionalLed.setColor(BidirectionalLed::Color::OFF);
        this->setState(State::INITIALIZATION_WAIT);
        return;
    }

    if (!button.isPressed() && !extraButton.isPressed()) {
        bidirectionalLed.setColor(BidirectionalLed::Color::OFF);
        return;
    }

    if (button.isEvent() && button.isPressed()) {
        this->isTurnLeftFork_[buttonCounter_] = false;
        bidirectionalLed.setColor(BidirectionalLed::Color::GREEN);
        ++this->buttonCounter_;
    } else if (extraButton.isEvent() && extraButton.isPressed()) {
        this->isTurnLeftFork_[buttonCounter_] = true;
        bidirectionalLed.setColor(BidirectionalLed::Color::RED);
        ++this->buttonCounter_;
    }

    button.consumeEvent();
    extraButton.consumeEvent();
}

void Challenge::initializationWaitHandler(uint16_t deltaTimeMs) {
    if (this->switchedState_) {
        this->initializationSleepTimeLeft_ = INITIALIZATION_SLEEP_TIME;
        return;
    }

    this->initializationSleepTimeLeft_ =
        cappingSubtract(this->initializationSleepTimeLeft_, deltaTimeMs);
    if (this->initializationSleepTimeLeft_ == 0) {
        this->setState(State::LOCATE);
    }
}

void Challenge::nextStateHandler() {
    if (this->switchedState_) {
        this->nextStateStep_ = 0;
    }

    if (this->nextStateStep_ != 0 && !this->lineFollower_.isLost())
        return;

    LineFollowerConfiguration configuration{
        .state = LineFollowerState::LOST,
        .isAutomatic = true,
        .isAlignAfterTurn = false,
        .isEventOnThree = true,
        .isSkippingStartingLine = false,
        .adjustTimeMs =
            LineFollowerConfiguration::TURN_WHEEL_ADJUST_TIME_LONG_MS};

    switch (this->previousState_) {
        case State::FORK_CHALLENGE:
            if (nextStateStep_ == 0) {
                configuration.isSkippingStartingLine = true;
                configuration.isAlignAfterTurn = true;
                configuration.state = LineFollowerState::TURNING_RIGHT;
            } else {
                this->setState(State::HOUSE_CHALLENGE);
            }
            break;
        case State::HOUSE_CHALLENGE:
        case State::MAZE_CHALLENGE:
            if (nextStateStep_ == 0) {
                configuration.state = LineFollowerState::FORWARD;
            } else if (nextStateStep_ == 1) {
                configuration.state = LineFollowerState::TURNING_RIGHT;
                configuration.isAutomatic = this->challengeStateTracker_ != 3;
                configuration.isEventOnThree = false;
            } else {
                if (this->challengeStateTracker_ == 3)
                    this->setState(State::FINISH);
                else
                    this->setState(previousState_ == State::HOUSE_CHALLENGE
                                       ? State::MAZE_CHALLENGE
                                       : State::FORK_CHALLENGE);
            }

            break;
        default:
            break;
    }

    ++nextStateStep_;

    this->lineFollower_.start(configuration);
}

void Challenge::locateHandler() {
    if (this->switchedState_) {
        this->lineFollower_.start(
            LineFollowerConfiguration{.state = LineFollowerState::FORWARD,
                                      .isEventOnThree = false});
    } else if (this->lineFollower_.isLost()) {
        const uint8_t darkLineCount =
            Robot::get().getLineSensor().getReadings().getDarkLineCount();

        this->setState(darkLineCount == 0 ? State::FORK_CHALLENGE
                                          : State::MAZE_CHALLENGE);
    }
}

void Challenge::finishHandler() {
    constexpr const uint16_t FREQUENCY = 2U;
    constexpr const uint16_t MS_IN_S = 1000U;
    constexpr const uint16_t HALF_PERIOD = MS_IN_S / FREQUENCY;

    Robot::get().getMovementManager().stop();

    Robot::get().getBidirectionalLed().setColor(BidirectionalLed::Color::RED);
    Board::get().getWatchdogTimer().sleep(HALF_PERIOD,
                                          WatchdogTimer::SleepMode::IDLE);
    Robot::get().getBidirectionalLed().setColor(BidirectionalLed::Color::GREEN);
    Board::get().getWatchdogTimer().sleep(HALF_PERIOD,
                                          WatchdogTimer::SleepMode::IDLE);
}

Challenge::StateHolder::StateHolder(State state) : state(state) {
    switch (state) {
        case State::FORK_CHALLENGE:
            this->handler.fork = ForkChallengeHandler();
            break;
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
            break;
        case Challenge::State::FORK_CHALLENGE:
            handler.fork.~ForkChallengeHandler();
            break;
        default:
            break;
    }
}
