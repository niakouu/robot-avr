#ifndef _MOVEMENT_MANAGER_H
#define _MOVEMENT_MANAGER_H

#include "Motor.h"
#include "Timer.h"

template <typename T>
class MovementManager
{
public:
    MovementManager();
    ~MovementManager();

    void moveForward(float speed) const;
    void moveBackward(float speed) const;
    void moveLeft(float speed, float curveRatio) const;
    void moveRight(float speed, float curveRatio) const;
    void stop() const;

private:
    Timer<T>* timer;
    Motor motorLeft_, motorRight_;
};

#endif /* _MOVEMENT_MANAGER_H */
