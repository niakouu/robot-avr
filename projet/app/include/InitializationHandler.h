#ifndef _INITIALIZATION_HANDLER_H
#define _INITIALIZATION_HANDLER_H

#include <stdint.h>

#include "ChallengeHandler.h"

class InitializationHandler : ChallengeHandler {
public:
    InitializationHandler();
    ~InitializationHandler() = default;
    void update(uint16_t deltaTimeMs, Challenge& challenge) override;

    static bool isBPointNorth;
    static bool isCPointNorth;

private:
    enum class Point : uint8_t { B, C, EXIT };

    Point currentState_;
    uint8_t counter_;

    void chooseDirection(bool& isPointNorth);
    void endingPointHandler(Challenge& challenge);
};

#endif /* _INITIALIZATION_HANDLER_H*/