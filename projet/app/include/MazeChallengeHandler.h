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