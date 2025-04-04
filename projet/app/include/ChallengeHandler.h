#ifndef _CHALLENGE_HANDLER_H
#define _CHALLENGE_HANDLER_H

#include <stdint.h>

class ChallengeHandler {
public:
    virtual void enter() = 0;

    virtual void update(uint16_t deltaTimeMs) = 0;
};

#endif /* _CHALLENGE_HANDLER_H */