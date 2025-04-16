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
// 

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
