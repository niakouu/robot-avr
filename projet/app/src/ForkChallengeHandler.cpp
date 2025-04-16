#include "ForkChallengeHandler.h"

#include "Challenge.h"
#include "Robot.h"

ForkChallengeHandler::ForkChallengeHandler()
    : currentState_(ForkChallengeHandler::Point::B_SOUND), counter_(0),
      isDone_(false), counterMidiMs_(MIDI_TIME_MS) {}

void ForkChallengeHandler::update(uint16_t deltaTimeMs, Challenge& challenge) {
    LineFollower<uint8_t, TimerPrescalerSynchronous>& lineFollower =
        challenge.getLineFollower();

    if (!lineFollower.isLost())
        return;

    LineFollowerConfiguration configuration{
        .state = LineFollowerState::LOST,
        .isAutomatic = true,
        .isAlignAfterTurn = false,
        .isEventOnThree = false,
        .isSkippingStartingLine = false,
        .adjustTimeMs =
            LineFollowerConfiguration::TURN_WHEEL_ADJUST_TIME_SHORT_MS};

    const uint8_t darkLineCount =
        Robot::get().getLineSensor().getReadings().getDarkLineCount();

    switch (this->currentState_) {
        case Point::B_SOUND:
            if (this->counterMidiMs_ == 0) {
                Robot::get().getMidi().stop();
                this->counterMidiMs_ = MIDI_TIME_MS;
                currentState_ = Point::B;
                return;
            }

            if (this->counterMidiMs_ == MIDI_TIME_MS) {
                Robot::get().getMidi().playNote(LOW_PITCH_TONE);
            }

            this->counterMidiMs_ =
                ::saturatingSubtract(this->counterMidiMs_, deltaTimeMs);

            break;
        case Point::B:
            currentState_ = Point::B_TO_C;
            configuration.isEventOnThree = true;

            if (Challenge::get().isTurnLeftFork(true)) {
                configuration.state = LineFollowerState::TURNING_LEFT;
            } else {
                configuration.state = LineFollowerState::TURNING_RIGHT;
            }

            break;

        case Point::B_TO_C:
            configuration.state = LineFollowerState::FORWARD;
            if (darkLineCount == 0) {
                configuration.state = LineFollowerState::LOST;
                currentState_ = Point::C_SOUND;
            }

            break;
        case Point::C_SOUND:
            if (this->counterMidiMs_ == 0) {
                Robot::get().getMidi().stop();
                this->counterMidiMs_ = MIDI_TIME_MS;
                currentState_ = Point::C;
                return;
            }

            if (this->counterMidiMs_ == MIDI_TIME_MS) {
                Robot::get().getMidi().playNote(LOW_PITCH_TONE);
            }

            this->counterMidiMs_ =
                ::saturatingSubtract(this->counterMidiMs_, deltaTimeMs);

            break;
        case Point::C:
            configuration.isEventOnThree = false;
            if (Challenge::get().isTurnLeftFork(false)) {
                configuration.state = LineFollowerState::TURNING_LEFT;
            } else {
                configuration.state = LineFollowerState::TURNING_RIGHT;
            }

            currentState_ = Point::C_TO_TURN;
            break;
        case Point::C_TO_TURN:
            configuration.isEventOnThree = false;
            if (Challenge::get().isTurnLeftFork(false)) {
                configuration.state = LineFollowerState::TURNING_RIGHT;
            } else {
                configuration.state = LineFollowerState::TURNING_LEFT;
            }

            currentState_ = Point::EXIT;

            break;
        case Point::EXIT:
            configuration.state = LineFollowerState::LOST;
            this->isDone_ = true;
            break;
        default:
            break;
    }

    lineFollower.start(configuration);
}

bool ForkChallengeHandler::isDone() {
    return this->isDone_;
}