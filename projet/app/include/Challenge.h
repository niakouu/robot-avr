#ifndef _CHALLENGE_H
#define _CHALLENGE_H

#include "stdint.h"
#include "Robot.h"

class Challenge {
public:
static Challenge& get();
Challenge(Challenge&) = delete;
void operator=(const Challenge&) = delete;

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
    static Challenge challenge_;

    static void initiazliationHandler();
    static void followLineHandler();
    static void locateHandler();
    static void forkChallengeHandler();
    static void houseChallengeHandler();
    static void mazeChallengeHandler();
    static void parkHandler();
    static void finishHandler();

    Challenge() noexcept;
    ~Challenge() = default;
};

#endif /* _CHALLENGE_H */