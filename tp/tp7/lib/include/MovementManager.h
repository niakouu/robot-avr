#ifndef _MOVEMENT_MANAGER_H
#define _MOVEMENT_MANAGER_H

#include "Motor.h"
#include "Timer.h"

template <typename T, typename U>
class MovementManager {
public:
    MovementManager(Timer<T, U>& timer, const Motor<T>&& motorLeft,
                    const Motor<T>&& motorRight);
    ~MovementManager();

    void moveForward(float speed);
    void moveBackward(float speed);
    void moveLeft(float speed, float curveRatio);
    void moveRight(float speed, float curveRatio);
    void stop();

    void setMotorOffsets(float offsetLeft, float offsetRight);

private:
    Timer<T, U>& timer_;
    Motor<T> motorLeft_, motorRight_;
};

#endif /* _MOVEMENT_MANAGER_H */
