//
// Travail : Projet Final
// Section # : 4
// Équipe # : 8290
// Correcteur : Dorine Dantrun
// ----------------------------------------------------------
// Développeurs : Edelina Alieva, Kyle Bouchard, Ilyes Belhaddad et Mohamed
// Elyes Bradai
//
// Description du Challenge.h :
//        Détiens le switch-case le plus important de notre programme qui
//        s'occupe de changer d'un challenge à un autre à chaque update.
//        Également, il s'occupe de suivre la ligne correctement durant le long
//        du parcours. La classe est un singleton.
//
// Identification matérielles : Voir dans main.cpp
//
// Table des états :
// +-----------------------+-------------------------------+------------------------+
// | Current State         | Condition                     | Next State             |
// +-----------------------+-------------------------------+------------------------+
// | INITIALIZATION        | Button pressed (2x)           | INITIALIZATION_WAIT    |
// | INITIALIZATION        | Else                          | INITIALIZATION         |
// +-----------------------+-------------------------------+------------------------+
// | INITIALIZATION_WAIT   | Delay elapsed (2s)            | LOCATE                 |
// | INITIALIZATION_WAIT   | Else                          | INITIALIZATION_WAIT    |
// +-----------------------+-------------------------------+------------------------+
// | LOCATE                | isLost() && darkLineCount = 0 | FORK_CHALLENGE         |
// | LOCATE                | isLost() && darkLineCount > 0 | MAZE_CHALLENGE         |
// | LOCATE                | Else                          | LOCATE                 |
// +-----------------------+-------------------------------+------------------------+
// | FORK_CHALLENGE        | fork.isDone()                 | NEXT_STATE             |
// | FORK_CHALLENGE        | Else                          | FORK_CHALLENGE         |
// +-----------------------+-------------------------------+------------------------+
// | HOUSE_CHALLENGE       | house.isDone()                | NEXT_STATE             |
// | HOUSE_CHALLENGE       | Else                          | HOUSE_CHALLENGE        |
// +-----------------------+-------------------------------+------------------------+
// | MAZE_CHALLENGE        | maze.isDone()                 | NEXT_STATE             |
// | MAZE_CHALLENGE        | Else                          | MAZE_CHALLENGE         |
// +-----------------------+-------------------------------+------------------------+
// | NEXT_STATE            | prev == FORK_CHALLENGE        |                        |
// |                       |     step == 0                 | NEXT_STATE             |
// |                       |     step == 1                 | HOUSE_CHALLENGE        |
// | NEXT_STATE            | prev == HOUSE_CHALLENGE       |                        |
// |                       |     step == 0, step == 1      | NEXT_STATE             |
// |                       |     step == 3 && done == 3    | FINISH                 |
// |                       |     step == 3 && done != 3    | MAZE_CHALLENGE         |
// |                       | Else                          | NEXT_STATE             |
// | NEXT_STATE            | prev == MAZE_CHALLENGE        |                        |
// |                       |     step == 0, step == 1      | NEXT_STATE             |
// |                       |     step == 3 && done == 3    | FINISH                 |
// |                       |     step == 3 && done != 3    | FORK_CHALLENGE         |
// |                       | Else                          | NEXT_STATE             |
// +-----------------------+-------------------------------+------------------------+
// | FINISH                | Periodic (every 0.5s)         | FINISH                 |
// +-----------------------+-------------------------------+------------------------+


#ifndef _CHALLENGE_H
#define _CHALLENGE_H

#include <stdint.h>

#include "ChallengeHandler.h"
#include "ForkChallengeHandler.h"
#include "HouseChallengeHandler.h"
#include "LineFollower.h"
#include "MazeChallengeHandler.h"
#include "Robot.h"

class Challenge {
public:
    static Challenge& get();
    Challenge(Challenge&) = delete;
    void operator=(const Challenge&) = delete;

    enum class State : uint8_t {
        INITIALIZATION,
        INITIALIZATION_WAIT,
        NEXT_STATE,
        LOCATE,
        FORK_CHALLENGE,
        HOUSE_CHALLENGE,
        MAZE_CHALLENGE,
        FINISH
    };

    void update(uint16_t deltaTimeMs);

    LineFollower<uint8_t, TimerPrescalerSynchronous>& getLineFollower();

    static constexpr float SPEED = 0.5F;

    bool isTurnLeftFork(bool first) const;

private:
    static Challenge challenge_;

    static constexpr const uint16_t INITIALIZATION_SLEEP_TIME = 2000U;

    bool isTurnLeftFork_[2];
    uint8_t challengeStateTracker_, buttonCounter_, nextStateStep_;
    LineFollower<uint8_t, TimerPrescalerSynchronous> lineFollower_;
    bool switchedState_;
    State previousState_;
    uint16_t initializationSleepTimeLeft_;

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

    void initializationHandler();
    void initializationWaitHandler(uint16_t deltaTimeMs);
    void nextStateHandler();
    void locateHandler();
    void finishHandler();

    void setState(State state);

    Challenge() noexcept;
    ~Challenge() = default;
};

#endif /* _CHALLENGE_H */
