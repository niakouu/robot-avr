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
    static const constexpr uint8_t LOW_PITCH_TONE = 45;
    enum class Point : uint8_t {
        B_SOUND,
        B,
        B_TO_C,
        C_SOUND,
        C,
        C_TO_TURN,
        TURN_TO_D,
        EXIT
    };

    Point currentState_;
    uint8_t counter_;
    bool isDone_;
    uint16_t counterMidiMs_;

    static constexpr const uint16_t MIDI_TIME_MS = 1000;
};

#endif /* _FORK_CHALLENGE_HANDLER_H */