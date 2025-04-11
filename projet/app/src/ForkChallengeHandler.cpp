#include "ForkChallengeHandler.h"

#include "Challenge.h"
#include "InitializationHandler.h"
#include "Robot.h"

ForkChallengeHandler::ForkChallengeHandler()
    : currentState_(ForkChallengeHandler::Point::BSound), counter_(0),
      counterMidiMs(MIDI_TIME_MS) {}

void ForkChallengeHandler::update(uint16_t deltaTimeMs, Challenge& challenge) {
    LineFollower<uint8_t, TimerPrescalerSynchronous>& lineFollower =
        challenge.getLineFollower();

    // if (!lineFollower.isLost())
    //     return;

    LineFollowerConfiguration configuration{.isAutomatic = true,
                                            .isEventOnThree = true,
                                            .isTurnInPlace = false,
                                            .isSkippingLine = false};

    printf("CounterMIDI: %d\n", counterMidiMs);

    switch (this->currentState_) {
        case Point::BSound:
            printf("ON EST DANS BSound\n");
            if (this->counterMidiMs == MIDI_TIME_MS) {
                Robot::get().getMidi().playNote(80);
            }
            
            if (this->counterMidiMs != 0) {
                this->counterMidiMs =
                    cappingSubtract(this->counterMidiMs, deltaTimeMs);
            } else {
                Robot::get().getMidi().stop();
                currentState_ = Point::B;
            }
            break;
        case Point::B:
            printf("ON EST DANS B\n");

            if (InitializationHandler::isBPointNorth)
                configuration.state = LineFollowerState::TURNING_LEFT;
            else
                configuration.state = LineFollowerState::TURNING_RIGHT;

            if (configuration.state == LineFollowerState::LOST)
                currentState_ = Point::BToC;

            break;
        case Point::BToC:
            printf("ON EST DANS BToC\n");

            configuration.isEventOnThree = false;
            configuration.state = LineFollowerState::FORWARD;
            if (configuration.state == LineFollowerState::LOST)
                currentState_ = Point::CSound;
            break;
        case Point::CSound:
            printf("ON EST DANS CSound\n");

            Robot::get().getMidi().playNote(80);
            Board::get().getWatchdogTimer().sleep(
                1000, WatchdogTimer::SleepMode::IDLE);
            Robot::get().getMidi().stop();
            currentState_ = Point::C;
            break;
        case Point::C:
            printf("ON EST DANS C\n");

            configuration.isEventOnThree = true;
            if (InitializationHandler::isCPointNorth) {
                configuration.state = LineFollowerState::TURNING_LEFT;
            } else {
                configuration.state = LineFollowerState::TURNING_RIGHT;
            }
            if (configuration.state == LineFollowerState::LOST)
                currentState_ = Point::CToD;
            break;
        case Point::CToD:
            printf("ON EST DANS CToD\n");

            configuration.isEventOnThree = false;
            configuration.state = LineFollowerState::FORWARD;
            if (configuration.state == LineFollowerState::LOST)
                currentState_ = Point::EXIT;
            break;
        case Point::EXIT:
            printf("ON EST DANS EXIT\n");

            endingPointHandler(challenge);
            break;
        default:
            break;
    }
}

void ForkChallengeHandler::endingPointHandler(Challenge& challenge) {
    challenge.setState(Challenge::State::FOLLOW_LINE);
}