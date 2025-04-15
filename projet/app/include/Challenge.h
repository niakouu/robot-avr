#ifndef _CHALLENGE_H
#define _CHALLENGE_H

#include <stdint.h>

#include "ChallengeHandler.h"
#include "HouseChallengeHandler.h"
#include "MazeChallengeHandler.h"
#include "ForkChallengeHandler.h"
#include "LineFollower.h"
#include "Robot.h"

class Challenge {
public:
    static Challenge& get();
    Challenge(Challenge&) = delete;
    void operator=(const Challenge&) = delete;

    enum class State : uint8_t {
        INITIALIZATION,
        NEXT_STATE,
        LOCATE,
        FORK_CHALLENGE,
        HOUSE_CHALLENGE,
        MAZE_CHALLENGE,
        PARK,
        FINISH
    };

    void update(uint16_t deltaTimeMs);

    LineFollower<uint8_t, TimerPrescalerSynchronous>& getLineFollower();

    static constexpr float SPEED = 0.5F;

    bool isTurnLeftFork(bool first) const;

private:
    static Challenge challenge_;

    bool isTurnLeftFork_[2];
    uint8_t challengeStateTracker_, buttonCounter_, nextStateStep_;
    LineFollower<uint8_t, TimerPrescalerSynchronous> lineFollower_;
    bool switchedState_;
    State previousState_;

    struct StateHolder {
        State state;

        union Handler {
            uint8_t none;
            HouseChallengeHandler house;
            MazeChallengeHandler maze;
            ForkChallengeHandler fork;

            constexpr Handler() : none{0} {};
            ~Handler() {};
        } handler;

        StateHolder(State state);
        ~StateHolder();
    } stateHolder_;

    void initiazliationHandler();
    void nextStateHandler();
    void locateHandler();
    void forkChallengeHandler();
    void parkHandler();
    void finishHandler();

    void setState(State state);

    Challenge() noexcept;
    ~Challenge() = default;
};

#endif /* _CHALLENGE_H */
