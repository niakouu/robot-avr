#ifndef _HOUSE_CHALLENGE_HANDLER_H
#define _HOUSE_CHALLENGE_HANDLER_H

#include "ChallengeHandler.h"

class HouseChallengeHandler : ChallengeHandler
{
public:
    HouseChallengeHandler();
    ~HouseChallengeHandler();

    void update(uint16_t deltaTimeMs, Challenge& challenge) override;
private:
    enum class Point : uint8_t {
        E_INITIAL, F, G, G_CHECK, H, I, E_FINAL, F_FINAL
    };

    static constexpr uint16_t SWEEP_TIME_MS = 1000;
    static constexpr uint8_t POLE_READING_COUNT = 10;

    void sweepForPole(uint16_t deltaTimeMs);

    Point point_;
    bool isPolePresent_;
    uint16_t sweepTimeLeftMs_;

    uint8_t averagePoleDistance_, totalReadings_;
};

#endif /* _HOUSE_CHALLENGE_HANDLER_H */