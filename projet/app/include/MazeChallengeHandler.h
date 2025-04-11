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

private:
    enum class Point : uint8_t {
        ENTRY_ONE,
        ENTRY_TWO,
        ENTRY_THREE,
        ENTRY_TO_K,
        ENTRY_TO_M,
        K,
        L,
        M,
        BETWEEN_K_N,
        BETWEEN_M_P,
        K_TO_N,
        M_TO_P,
        SECOND_ENTRY_ONE,
        SECOND_ENTRY_TWO,
        SECOND_ENTRY_THREE,
        SECOND_ENTRY_TO_N,
        SECOND_ENTRY_TO_P,
        N,
        O,
        P,
        LAST,
        EXIT
    };

    static constexpr uint16_t TURN_TIME_MS = 250U;
    static constexpr uint8_t DISTANCE_TO_CENTER = 33U;
    static constexpr uint8_t DISTANCE_TO_DIAGONAL = 33U;
    static constexpr uint8_t POLE_READING_COUNT = 10U;

    Point currentPoint_;
    uint16_t rotateTimeLeftMs_, averagePoleDistance_;
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

    friend const char *toString(Orientation);
    friend const char *toString(Lane);
    friend const char *toString(Stage);

    Stage currentStage : 2;
    Orientation orientation_ : 2;
    Lane lane_ : 2;
    bool isIntermediateStep_ : 1;
    bool isCheckingInPosition_ : 1;

    bool rotate(bool left, uint16_t deltaTimeMs, uint16_t turnTimeMs);
    void endingPointHandler(Challenge& challenge,
                            LineFollowerConfiguration& configuration);

    Lane getFreeLane() const;


    bool handleNoPoles();
    bool handleCheckNextLane(LineFollowerConfiguration&, uint16_t deltaTimeMs);
    void handleCurrentLaneIsFreeLane(LineFollowerConfiguration& configuration);
    void handleNextLaneIsFreeLane(LineFollowerConfiguration& configuration);
    void handleFarLaneIsFreeLane(LineFollowerConfiguration& configuration);

    void updateOrientation(LineFollowerState nextState);

    bool isPolePresent(uint8_t distance) const;
    void calculatePoleDistance();
    void resetDistanceValues();
    bool execFirstDecision(
        uint16_t deltaTimeMs, LineFollowerConfiguration& configuration,
        Point forward, Point currentNext);
    void execSecondDecision(LineFollowerConfiguration& configuration,
                            Point left, Point currentNext);
    void detectPoleBetweenSidePoints(LineFollowerConfiguration& configuration,
                                     Point forward, Point side);
    void switchPoint(Point nextPoint, LineFollowerState nextLineState,
                     LineFollowerConfiguration& configuration);
};

#endif /* _MAZE_CHALLENGE_HANDLER_H */