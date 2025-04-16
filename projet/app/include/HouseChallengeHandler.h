//
// Travail : Projet Final
// Section # : 4
// Équipe # : 8290
// Correcteur : Dorine Dantrun
// ----------------------------------------------------------
// Développeurs : Edelina Alieva, Kyle Bouchard, Ilyes Belhaddad et Mohamed Elyes Bradai
// Description du HouseChallengeHandler.h : Après avoir détecté le point E, le robot se dirige vers
//                                         le point F, puis vers le point G. Arrivé au point G, le robot
//                                         utilise son capteur de distance pour vérifier s'il y a un poteau au
//                                         point H ou pas. Si le robot détecte un poteau au point H, la LED clignote à 4 Hz 
//                                         pendant 2 secondes en rouge puis se dirige vers le point I. Si le robot ne détecte 
//                                         pas de poteau au point H, la LED clignote à 4 Hz pendant 2 secondes en vert puis se 
//                                         dirige vers le point H puis le point I. Le robot se dirige ensuite vers le point E, 
//                                         puis F et se dirige vers le point J.
// Identification matérielles : Voir main.cpp
// Table des états :
// | Current State   | Condition                                          | Next State    |
// |-----------------|----------------------------------------------------|---------------|
// | E_INITIAL       | Line follower detects end of segment               | F             |
// | F               | Line follower detects end of segment               | G             |
// | G               | Line follower detects end of segment               | G_CHECK       |
// | G_CHECK         | distance <= POLE_DISTANCE                          | I_FROM_G      |
// | G_CHECK         | distance > POLE_DISTANCE                           | H             |
// | H               | Line follower detects end of segment               | I_FROM_H      |
// | I_FROM_H        | Line follower detects end of segment               | E_FINAL       |
// | I_FROM_G        | Line follower detects end of segment               | E_FINAL       |
// | E_FINAL         | Line follower detects end of segment               | F_FINAL       |
// | F_FINAL         | Line follower detects end of segment               | F_FINAL       |

#ifndef _HOUSE_CHALLENGE_HANDLER_H
#define _HOUSE_CHALLENGE_HANDLER_H

#include "ChallengeHandler.h"
#include "Led.h"

class HouseChallengeHandler : ChallengeHandler {
public:
    HouseChallengeHandler();
    ~HouseChallengeHandler() = default;

    void update(uint16_t deltaTimeMs, Challenge& challenge) override;
    bool isDone() override;

private:
    enum class Point : uint8_t {
        E_INITIAL,
        F,
        G,
        G_CHECK,
        H,
        I_FROM_G,
        I_FROM_H,
        E_FINAL,
        F_FINAL
    };

    static constexpr uint8_t POLE_READING_COUNT = 10U;
    static constexpr uint8_t FLASH_FREQ = 4U;
    static constexpr uint16_t FLASH_DURATION_MS = 2000U;
    static constexpr uint8_t POLE_DISTANCE = 35U;

    Point point_;
    bool isPolePresent_, isDone_;

    uint8_t minimumPoleDistance_;
    uint8_t totalReadings_;

    static void flashLed(BidirectionalLed::Color color);
};

#endif /* _HOUSE_CHALLENGE_HANDLER_H */