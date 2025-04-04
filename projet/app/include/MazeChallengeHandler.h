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
    enum class Point : uint8_t { ENTRY, K, L, M, SECOND_ENTRY, N, O, P, EXIT };

    static constexpr uint16_t SWEEP_TIME_MS = 1500U;
    static constexpr uint16_t TURN_TIME_MS = 1000U;
    static constexpr uint16_t SWEEP_TIME_OFFSET_MS = SWEEP_TIME_MS / 3U;
    static constexpr uint8_t DISTANCE_TO_CENTER = 33U;
    static constexpr uint8_t DISTANCE_TO_DIAGONAL = 41U;
    static constexpr uint8_t FIRST_LOST_COUNTER = 4U;

    Point currentState_;
    uint16_t rotateTimeLeftMs_, sweepTimeLeftMs_;
    uint8_t counter_;

    void determinePointState(uint16_t deltaTimeMs, Point leftPoint, Point rightPoint);
    void rotate(bool left, uint16_t deltaTimeMs, uint16_t turnTimeMs);
    void firstStagePointHandler(bool upperPoint, LineFollower<uint8_t, TimerPrescalerSynchronous>& lineFollower, Point nextPoint);
    void secondStagePointHandler(LineFollower<uint8_t, TimerPrescalerSynchronous>& lineFollower);
    void pointLHandler(LineFollower<uint8_t, TimerPrescalerSynchronous>& lineFollower);
    void endingPointHandler(Challenge& challenge, LineFollower<uint8_t, TimerPrescalerSynchronous>& lineFollower);

    static bool isPolePresent(uint8_t distance);
};

#endif /* _MAZE_CHALLENGE_HANDLER_H */