#ifndef _MOVEMENT_MANAGER_H
#define _MOVEMENT_MANAGER_H

#include "Motor.h"
#include "Timer.h"

template <typename T, typename U>
class MovementManager {
public:
    MovementManager(const Timer<T, U>& timer, const Motor&& motorLeft,
                    const Motor&& motorRight);
    ~MovementManager();

    void moveForward(float speed) const;
    void moveBackward(float speed) const;
    void moveLeft(float speed, float curveRatio) const;
    void moveRight(float speed, float curveRatio) const;
    void stop() const;

    void setMotorOffsets(float offsetLeft, float offsetRight);

private:
    const Timer<T, U>& timer_;
    Motor motorLeft_, motorRight_;
};

#endif /* _MOVEMENT_MANAGER_H */
