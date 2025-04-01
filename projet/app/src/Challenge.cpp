#include "Challenge.h"

Challenge::Challenge() : currentState_(State::INITIALIZATION) {}

Challenge& Challenge::get() {
    return Challenge::challenge_;
}
void Challenge::startChallenge() {

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
            break;
        case State::HOUSE_CHALLENGE:
            houseChallengeHandler();
            break;
        case State::MAZE_CHALLENGE:
            mazeChallengeHandler();
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

void Challenge::finishHandler() {}