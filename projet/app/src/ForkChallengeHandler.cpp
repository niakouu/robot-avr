#include "ForkChallengeHandler.h"

#include "Challenge.h"
#include "Robot.h"

ForkChallengeHandler::ForkChallengeHandler()
    : currentState_(ForkChallengeHandler::Point::B), counter_(0) {}

void ForkChallengeHandler::update(uint16_t deltaTimeMs, Challenge& challenge) {
    LineFollower<uint8_t, TimerPrescalerSynchronous>& lineFollower =
        challenge.getLineFollower();

    if (!lineFollower.isLost())
        return;

    LineFollowerConfiguration configuration{.isAutomatic = true,
                                            .isEventOnThree = true,
                                            .isTurnInPlace = false,
                                            .isSkippingStartingLine = false};

    // printf("ON EST DANS FORK\n");

    switch (this->currentState_) {
        case ForkChallengeHandler::Point::BSound:
            // TODO SOUND
            currentState_ = ForkChallengeHandler::Point::B;
            break;
        case ForkChallengeHandler::Point::B:
            // TODO SON
            //  goNextPoint(InitializationHandler::isBPointNorth, lineFollower,
            //  challenge);

            // if (InitializationHandler::isBPointNorth) {
            //     configuration.state = LineFollowerState::TURNING_LEFT;
            // } else {
            //     configuration.state = LineFollowerState::TURNING_RIGHT;
            // }
            // if (configuration.state == LineFollowerState::LOST)
            //     currentState_ = ForkChallengeHandler::Point::BToC;
            break;
        case ForkChallengeHandler::Point::BToC:
            configuration.isEventOnThree = false;
            configuration.state = LineFollowerState::FORWARD;
            if (configuration.state == LineFollowerState::LOST)
                currentState_ = ForkChallengeHandler::Point::CSound;
            break;
        case ForkChallengeHandler::Point::CSound:
            // TODO SOUND
            currentState_ = ForkChallengeHandler::Point::C;
            break;
        case ForkChallengeHandler::Point::C:
            configuration.isEventOnThree = true;
            // if (InitializationHandler::isCPointNorth) {
            //     configuration.state = LineFollowerState::TURNING_LEFT;
            // } else {
            //     configuration.state = LineFollowerState::TURNING_RIGHT;
            // }
            // if (configuration.state == LineFollowerState::LOST)
            //     currentState_ = ForkChallengeHandler::Point::CToD;
            break;
        case ForkChallengeHandler::Point::CToD:
            configuration.isEventOnThree = false;
            configuration.state = LineFollowerState::FORWARD;
            if (configuration.state == LineFollowerState::LOST)
                currentState_ = ForkChallengeHandler::Point::EXIT;
            break;
        case ForkChallengeHandler::Point::EXIT:
            endingPointHandler(challenge);
            break;
        default:
            break;
    }
}

// void ForkChallengeHandler::goNextPoint(bool& isPointNorth,
// LineFollower<uint8_t, TimerPrescalerSynchronous>& lineFollower, Challenge&
// challenge){

//     printf("ON EST DANS GONEXTPOINT\n");

//     if(isPointNorth){
//         configuration.state = LineFollowerState::TURNING_LEFT;
//     }else{
//         configuration.state = LineFollowerState::TURNING_RIGHT;
//     }

// }

void ForkChallengeHandler::endingPointHandler(Challenge& challenge) {
    challenge.setState(Challenge::State::FOLLOW_LINE);
}