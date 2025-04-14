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
    static constexpr uint16_t TURN_TIME_MS = 250U;
    static constexpr uint8_t DISTANCE_TO_CENTER = 45U;
    static constexpr uint8_t DISTANCE_TO_DIAGONAL = 45U;
    static constexpr uint8_t POLE_READING_COUNT = 10U;
    static constexpr uint8_t FLASH_FREQ = 4U;
    static constexpr uint16_t FLASH_DURATION_MS = 2000U;

    uint16_t rotateTimeLeftMs_, averagePoleDistance_, detectionDanceTimeLeftMs_;
    uint8_t counter_, totalReadings_;
    bool finishedCalculatingPole_;

    struct PoleMap {
        bool left : 1;
        bool center : 1;
        bool right : 1;
    } poleMap_;

    enum class Stage : uint8_t {
        Stage1,
        Stage2,
        End
    };

    enum class Lane : uint8_t {
        Left,
        Center,
        Right,
        Invalid
    };

    enum class Orientation : uint8_t {
        Left,
        Forward,
        Right,
    };

    Stage currentStage_ : 2;
    Orientation orientation_ : 2;
    Lane lane_ : 2;
    bool isIntermediateStep_ : 1;
    bool isCheckingInPosition_ : 1;
    bool isDone_ : 1;

    Lane getFreeLane() const;

    bool handleDetection(LineFollowerConfiguration& configuration);
    void handleCheckNextLane(LineFollowerConfiguration& configuration, const PoleMap& previousPoleMap);
    void handleCurrentLaneIsFreeLane(LineFollowerConfiguration& configuration, uint16_t deltaTimeMs);
    void handleNextLaneIsFreeLane(LineFollowerConfiguration& configuration);
    void handleFarLaneIsFreeLane(LineFollowerConfiguration& configuration);

    void updateOrientation(LineFollowerState nextState);

    bool isPolePresent(uint8_t distance) const;
    void calculatePoleDistance();
    void resetDistanceValues();

    bool handleDetectionDance(uint16_t deltaTimeMs);

    const char* toString(MazeChallengeHandler::Stage stage) {
        switch (stage) {
            case MazeChallengeHandler::Stage::Stage1:
                return "Stage1";
            case MazeChallengeHandler::Stage::Stage2:
                return "Stage2";
            case MazeChallengeHandler::Stage::End:
                return "End";
            default:
                return "UNKNOWN";
        }
    }
    
    const char* toString(LineFollowerState state) {
        switch (state) {
            case LineFollowerState::FORWARD:
                return "FORWARD";
            case LineFollowerState::ALIGN:
                return "ALIGN";
            case LineFollowerState::TURNING_LEFT:
                return "TURNING_LEFT";
            case LineFollowerState::TURNING_RIGHT:
                return "TURNING_RIGHT";
            case LineFollowerState::LOST:
                return "LOST";
            case LineFollowerState::STOP:
                return "STOP";
            default:
                return "UNKNOWN";
        }
    }
    
    const char* toString(MazeChallengeHandler::Lane lane) {
        switch (lane) {
            case MazeChallengeHandler::Lane::Left:
                return "Left";
            case MazeChallengeHandler::Lane::Center:
                return "Center";
            case MazeChallengeHandler::Lane::Right:
                return "Right";
            case MazeChallengeHandler::Lane::Invalid:
                return "Invalid";
            default:
                return "UNKNOWN";
        }
    }
    
    const char* toString(MazeChallengeHandler::Orientation orientation) {
        switch (orientation) {
            case MazeChallengeHandler::Orientation::Left:
                return "Left";
            case MazeChallengeHandler::Orientation::Forward:
                return "Forward";
            case MazeChallengeHandler::Orientation::Right:
                return "Right";
            default:
                return "UNKNOWN";
        }
    }
};

#endif /* _MAZE_CHALLENGE_HANDLER_H */