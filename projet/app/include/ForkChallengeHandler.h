//
// Travail : Projet Final
// Section # : 4
// Équipe # : 8290
// Correcteur : Dorine Dantrun
// ----------------------------------------------------------
// Développeurs : Edelina Alieva, Kyle Bouchard, Ilyes Belhaddad et Mohamed Elyes Bradai
// Description du ForkChallengeHandler.h : Après avoir détecté le point B, le robot s'arrête 
//                                         et fait un son grave d'une seconde, puis le robot décide
//                                         de tourner à droite ou à gauche selon les variables
//                                         qui ont été initialisées précédemment puis suit la ligne
//                                         jusqu'à arriver au point C. Au point C, le robot s'arrête 
//                                         et fait un son grave d'une seconde, puis le robot décide
//                                         de tourner à droite ou à gauche selon les variables
//                                         qui ont été initialisées précédemment puis suit la ligne
//                                         jusqu'à arriver au point D.
// Identification matérielles : Voir main.cpp
// Table des états : 
// +----------------+---------+----------------+
// | Current State  |  Lost   | Next State     |
// +----------------+---------+----------------+
// | B_SOUND        | X       | B              |
// | B              | X       | B_TO_C         |
// | B_TO_C         | 0       | B_TO_C         |
// | B_TO_C         | 1       | C_SOUND        |
// | C_SOUND        | X       | C              |
// | C              | X       | C_TO_TURN      |
// | C_TO_TURN      | 0       | C_TO_TURN      |
// | C_TO_TURN      | 1       | TURN_TO_D      |
// | TURN_TO_D      | 0       | TURN_TO_D      |
// | TURN_TO_D      | 1       | EXIT           |
// | EXIT           | X       | EXIT           |
// +----------------+---------+----------------+


#ifndef _FORK_CHALLENGE_HANDLER_H
#define _FORK_CHALLENGE_HANDLER_H

#include <stdint.h>

#include "ChallengeHandler.h"
#include "LineFollower.h"

class ForkChallengeHandler : ChallengeHandler {
public:
    ForkChallengeHandler();
    ~ForkChallengeHandler() = default;
    void update(uint16_t deltaTimeMs, Challenge& challenge) override;
    bool isDone() override;

private:
    static const constexpr uint8_t LOW_PITCH_TONE = 45;
    enum class Point : uint8_t {
        B_SOUND,
        B,
        B_TO_C,
        C_SOUND,
        C,
        C_TO_TURN,
        TURN_TO_D,
        EXIT
    };

    Point currentState_;
    uint8_t counter_;
    bool isDone_;
    uint16_t counterMidiMs_;

    static constexpr const uint16_t MIDI_TIME_MS = 1000;
};

#endif /* _FORK_CHALLENGE_HANDLER_H */