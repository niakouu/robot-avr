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

private:
    enum class Point : uint8_t { BSound, B, BToC, CSound, C, CToD, EXIT };

    Point currentState_;
    uint8_t counter_;


    void endingPointHandler(Challenge& challenge);
    uint16_t counterMidiMs;

    static constexpr const uint16_t MIDI_TIME_MS = 1000;
};

#endif /* _FORK_CHALLENGE_HANDLER_H */