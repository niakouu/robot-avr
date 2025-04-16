#include "ForkChallengeHandler.h"

#include "Challenge.h"
#include "InitializationHandler.h"
#include "Robot.h"

ForkChallengeHandler::ForkChallengeHandler()
    : currentState_(ForkChallengeHandler::Point::BSound), counter_(0),
      isDone_(false), counterMidiMs(MIDI_TIME_MS) {}

void ForkChallengeHandler::update(uint16_t deltaTimeMs, Challenge& challenge) {
    LineFollower<uint8_t, TimerPrescalerSynchronous>& lineFollower =
        challenge.getLineFollower();

    if (!lineFollower.isLost())
        return;

    LineFollowerConfiguration configuration{
        .state = LineFollowerState::LOST,
        .isAutomatic = true, // Continue toujours forward
        .isAlignAfterTurn = false,
        .isEventOnThree = false, // Lost aussi si 3 allum
        .isSkippingStartingLine = false,
        .adjustTimeMs =
            LineFollowerConfiguration::TURN_WHEEL_ADJUST_TIME_SHORT_MS};

    const uint8_t darkLineCount =
        Robot::get().getLineSensor().getReadings().getDarkLineCount();

    printf("s:%d\n", static_cast<uint8_t>(this->currentState_));
    switch (this->currentState_) {
        case Point::BSound:
            if (this->counterMidiMs == 0) {
                Robot::get().getMidi().stop();
                this->counterMidiMs = MIDI_TIME_MS;
                currentState_ = Point::B;
                return;
            }

            if (this->counterMidiMs == MIDI_TIME_MS) {
                Robot::get().getMidi().playNote(45);
            }

            this->counterMidiMs =
                cappingSubtract(this->counterMidiMs, deltaTimeMs);

            break;
        case Point::B:
            currentState_ = Point::BToC;
            configuration.isEventOnThree = true;

            if (Challenge::get().isTurnLeftFork(true)) {
                configuration.state = LineFollowerState::TURNING_LEFT;
            } else {
                configuration.state = LineFollowerState::TURNING_RIGHT;
            }

            break;

        case Point::BToC:
            configuration.state = LineFollowerState::FORWARD;
            if (darkLineCount == 0) {
                configuration.state = LineFollowerState::LOST;
                currentState_ = Point::CSound;
            }

            break;
        case Point::CSound:
            if (this->counterMidiMs == 0) {
                Robot::get().getMidi().stop();
                this->counterMidiMs = MIDI_TIME_MS;
                currentState_ = Point::C;
                return;
            }

            if (this->counterMidiMs == MIDI_TIME_MS) {
                Robot::get().getMidi().playNote(45);
            }

            this->counterMidiMs =
                cappingSubtract(this->counterMidiMs, deltaTimeMs);

            break;
        case Point::C:
            configuration.isEventOnThree = false;
            if (Challenge::get().isTurnLeftFork(false)) {
                configuration.state = LineFollowerState::TURNING_LEFT;
            } else {
                configuration.state = LineFollowerState::TURNING_RIGHT;
            }

            currentState_ = Point::CToTurn;
            break;
        case Point::CToTurn:
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
            endingPointHandler(challenge);
            break;
        default:
            break;
    }

    lineFollower.start(configuration);
}

bool ForkChallengeHandler::isDone() {
    return this->isDone_;
}

void ForkChallengeHandler::endingPointHandler(Challenge& challenge) {
    this->isDone_ = true;
}