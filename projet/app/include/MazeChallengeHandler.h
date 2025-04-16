//
// Travail : Projet Final
// Section # : 4
// Équipe # : 8290
// Correcteur : Dorine Dantrun
// ----------------------------------------------------------
// Développeurs : Edelina Alieva, Kyle Bouchard, Ilyes Belhaddad et Mohamed Elyes Bradai
// Description du MazeChallengeHandler.h :  Après avoir détecté la première intersection du Maze,
//                                          le robot se dirige vers les points M, L, K tout en utilisant son capteur
//                                          de distance pour détecter si le chemin est libre ou non. Dès qu’il identifie
//                                          le seul point sans poteau, il s’arrête, s’aligner avec ce point, puis fait 
//                                          clignoter sa DEL verte à une fréquence de 4 Hz pendant deux secondes. Ensuite, 
//                                          il avance pour traverser cette première ligne de la grille. Le même processus est
//                                          répété pour les points N, O, P. Si le robot détecte que deux chemins
//                                          sont bloqués, il en déduit automatiquement que le troisième est libre.
//                                          Après avoir franchi la grille, il continue son trajet en direction
//                                          du point A.
// Identification matérielles : Voir main.cpp
// Table des états : 
// | État actuel  | Orientation | Ligne actuelle    | Entrée / Condition                        | Action / Prochaine étape                           | Étape suivante                |
// |--------------|-------------|-------------------|-------------------------------------------|----------------------------------------------------|-------------------------------|
// | STAGE_1      | FORWARD     | CENTER            | 1er croisement détecté                    | Démarre la détection de poteau                     | handleDetection()             |
// | STAGE_1      | FORWARD     | LEFT/CENTER/RIGHT | Deux chemins bloqués, un libre            | Identifie la voie libre, se prépare à l’alignement | handleCurrentLaneIsFreeLane() |
// | STAGE_1      | FORWARD     | freeLane          | !isIntermediateStep_                      | Clignotement DEL verte (4 Hz, 2 sec)               | handleDetectionDance()        |
// | STAGE_1      | FORWARD     | freeLane          | isIntermediateStep_ devient true          | Avance vers ligne suivante                         | Avance à STAGE_2              |
// | STAGE_1 ou 2 | FORWARD     | N’importe         | freeLane != lane_ (1 saut latéral requis) | Tourne vers freeLane                               | handleNextLaneIsFreeLane()    |
// | STAGE_1 ou 2 | FORWARD     | N’importe         | freeLane opposé (ex: LEFT -> RIGHT)       | Tourne vers CENTER, puis re-détecte                | handleFarLaneIsFreeLane()     |
// | STAGE_1 ou 2 | FORWARD     | INVALID           | Aucune voie libre détectée                | Réorientation vers prochaine voie                  | handleCheckNextLane()         |
// | STAGE_2      | FORWARD     | freeLane          | Comme STAGE_1, après détection            | Traverse ligne vers A                              | currentStage_ = END           |
// | END          | RIGHT       | LEFT/CENTER       | Navigation vers point A                   | Suit trajectoire finale, se repositionne           | Arrêt (isDone_ = true)        |
// | END          | RIGHT       | RIGHT             | Dernière ligne atteinte                   | Stop complet                                       | LineFollowerState::STOP       |

#ifndef _MAZE_CHALLENGE_HANDLER_H
#define _MAZE_CHALLENGE_HANDLER_H

#include <stdint.h>

#include "ChallengeHandler.h"
#include "LineFollower.h"

class MazeChallengeHandler : ChallengeHandler {
public:
    MazeChallengeHandler();
    ~MazeChallengeHandler() = default;
    void update(uint16_t deltaTimeMs, Challenge& challenge) override;
    bool isDone() override;

private:
    static const constexpr uint16_t TURN_TIME_MS = 250U;
    static const constexpr uint8_t DISTANCE_TO_CENTER = 35U;
    static const constexpr uint8_t DISTANCE_TO_DIAGONAL = 35U;
    static const constexpr uint8_t POLE_READING_COUNT = 100U;
    static const constexpr uint16_t SWEEP_TIME_MS = 25U;
    static const constexpr uint8_t FLASH_FREQ = 4U;
    static const constexpr uint16_t FLASH_DURATION_MS = 2000U;
    static const constexpr float SWEEP_SPEED = 0.8F; 

    uint16_t rotateTimeLeftMs_, averagePoleDistance_, detectionDanceTimeLeftMs_;
    uint8_t counter_, totalReadings_;
    bool finishedCalculatingPole_;

    struct PoleMap {
        bool left : 1;
        bool center : 1;
        bool right : 1;
    } poleMap_;

    enum class Stage : uint8_t { STAGE_1, STAGE_2, END };

    enum class Lane : uint8_t { LEFT, CENTER, RIGHT, INVALID };

    enum class Orientation : uint8_t {
        LEFT,
        FORWARD,
        RIGHT,
    };

    Stage currentStage_ : 2;
    Orientation orientation_ : 2;
    Lane lane_ : 2;
    bool isIntermediateStep_ : 1;
    bool isCheckingInPosition_ : 1;
    bool isDone_ : 1;
    uint16_t sweepTimeLeft_;

    Lane getFreeLane() const;

    bool handleDetection(LineFollowerConfiguration& configuration,
                         uint16_t deltatimeMs, bool& isManual);
    void handleCheckNextLane(LineFollowerConfiguration& configuration,
                             const PoleMap& previousPoleMap);
    void handleCurrentLaneIsFreeLane(LineFollowerConfiguration& configuration,
                                     uint16_t deltaTimeMs);
    void handleNextLaneIsFreeLane(LineFollowerConfiguration& configuration);
    void handleFarLaneIsFreeLane(LineFollowerConfiguration& configuration);

    void updateOrientation(LineFollowerState nextState);

    bool isPolePresent(uint8_t distance) const;
    void calculatePoleDistance();
    void resetDistanceValues();

    bool handleDetectionDance(uint16_t deltaTimeMs);
};

#endif /* _MAZE_CHALLENGE_HANDLER_H */