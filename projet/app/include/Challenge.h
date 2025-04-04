#ifndef _CHALLENGE_H
#define _CHALLENGE_H

#include <stdint.h>

#include "LineFollower.h"
#include "Robot.h"
#include "ChallengeHandler.h"

class Challenge {
public:
    static Challenge& get();
    Challenge(Challenge&) = delete;
    void operator=(const Challenge&) = delete;

    void update(uint16_t deltaTimeMs);

private:
    static Challenge challenge_;

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
    uint8_t challengeStateTracker_;
    Robot& robot_;
    LineFollower<uint8_t, TimerPrescalerSynchronous> lineFollower_;

    static void initiazliationHandler();
    static void followLineHandler();
    static void locateHandler();
    static void forkChallengeHandler();
    static void houseChallengeHandler();
    static void mazeChallengeHandler();
    static void parkHandler();
    static void finishHandler();

    static constexpr float SPEED = 0.5F;

    Challenge() noexcept;
    ~Challenge() = default;
};

#endif /* _CHALLENGE_H */