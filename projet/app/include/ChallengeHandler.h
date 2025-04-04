#ifndef _CHALLENGE_HANDLER_H
#define _CHALLENGE_HANDLER_H

#include <stdint.h>

class Challenge;

class ChallengeHandler {
public:
    virtual void update(uint16_t deltaTimeMs, Challenge& challenge) = 0;
};

#endif /* _CHALLENGE_HANDLER_H */