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
    enum class Point : uint8_t { BSound,B,BToC,CSound,C,CToD, EXIT };


    Point currentState_;
    uint8_t counter_;

    void rotate(bool left, uint16_t deltaTimeMs, uint16_t turnTimeMs);
    void endingPointHandler(Challenge& challenge);
    void goNextPoint(bool& isPointNorth, LineFollower<uint8_t, TimerPrescalerSynchronous>& lineFollower, Challenge& challenge);

};

#endif /* _FORK_CHALLENGE_HANDLER_H */