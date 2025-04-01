#ifndef _CHALLENGE_H
#define _CHALLENGE_H

#include "stdint.h"

class Challenge {
public:
    Challenge();
    ~Challenge() = default;

    void startChallenge();

private:
    enum class State : uint8_t {
        INITIALIZATION,
        FOLLOW_LINE,
        LOCATE,
        FORK_CHALLENGE,
        HOUSE_CHALLENGE,
        MAZE_CHALLENGE,
        PARK,
        FINISH
    };
    State currentState_;

    static void initiazliationHandler();
    static void followLineHandler();
    static void locateHandler();
    static void forkChallengeHandler();
    static void houseChallengeHandler();
    static void mazeChallengeHandler();
    static void parkHandler();
    static void finishHandler();
};

#endif /* _CHALLENGE_H */