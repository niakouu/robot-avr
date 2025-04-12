#include "ForkChallengeHandler.h"

#include "Challenge.h"
#include "InitializationHandler.h"
#include "Robot.h"

ForkChallengeHandler::ForkChallengeHandler()
    : currentState_(ForkChallengeHandler::Point::BSound), counter_(0), /
      counterMidiMs(MIDI_TIME_MS) {}

void ForkChallengeHandler::update(uint16_t deltaTimeMs, Challenge& challenge) {
    LineFollower<uint8_t, TimerPrescalerSynchronous>& lineFollower =
        challenge.getLineFollower();

    if (!lineFollower.isLost())
        return;

    LineFollowerConfiguration configuration{
        .state = LineFollowerState::LOST,
        .isAutomatic = true,    // Continue toujours forward
        .isEventOnThree = true, // Lost aussi si 3 allum
        .isTurnInPlace = false, // surplace
        .isSkippingLine = false};

    // printf("CounterMIDI: %d\n", counterMidiMs);

    // configuration.state = LineFollowerState::TURNING_LEFT

    switch (this->currentState_) {
        case Point::BSound:
            printf("ON EST DANS BSound\n");

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
    
            printf("ON EST DANS B\n");
            if (true) { // InitializationHandler::isBPointNorth
                configuration.state = LineFollowerState::TURNING_LEFT;
                printf("ON EST DANS B EN TRAIN DE TOURNER A GAUCHE\n");
            } else {
                printf("ON EST DANS B EN TRAIN DE TOURNER A DROITE\n");
                configuration.state = LineFollowerState::TURNING_RIGHT;
            }

            break;

        case Point::BToC:
            printf("ON EST DANS BToC\n");

            configuration.isEventOnThree = false;
            configuration.state = LineFollowerState::FORWARD;
            currentState_ = Point::CSound;
            break;
        case Point::CSound:
            printf("ON EST DANS CSound\n");

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
            printf("ON EST DANS C\n");

            configuration.isEventOnThree = true;
            if (false) { // InitializationHandler::isCPointNorth
                configuration.state = LineFollowerState::TURNING_LEFT;
            } else {
                configuration.state = LineFollowerState::TURNING_RIGHT;
            }

            currentState_ = Point::CToTurn;
            break;
        case Point::CToTurn:
            printf("ON EST DANS CToTurn\n");
            
            if (false) { // !InitializationHandler::isCPointNorth
                configuration.state = LineFollowerState::TURNING_RIGHT;
            } else {
                configuration.state = LineFollowerState::TURNING_LEFT;
            }

            currentState_ = Point::EXIT;

            break;
        case Point::EXIT:
            printf("ON EST DANS EXIT\n");

            endingPointHandler(challenge);
            break;
        default:
            break;
    }

    printf("starting with config: %d\n", static_cast<uint8_t>(configuration.state));
    lineFollower.start(configuration);
}

void ForkChallengeHandler::endingPointHandler(Challenge& challenge) {
    challenge.setState(Challenge::State::FOLLOW_LINE);
}