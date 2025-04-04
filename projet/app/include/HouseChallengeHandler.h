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
        E_INITIAL, F, G, H, I, E_FINAL, F_FINAL
    };

    static constexpr uint16_t SWEEP_TIME_MS = 2000;

    void sweepForPole(uint16_t deltaTimeMs);

    Point point_;
    bool isPolePresent_;
    uint16_t sweepTimeLeftMs_;
};

#endif /* _HOUSE_CHALLENGE_HANDLER_H */