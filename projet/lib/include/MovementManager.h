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

    void moveForward(float speedRatio);
    void moveBackward(float speedRatio);
    void moveLeft(float speedRatio, float curveRatio);
    void moveRight(float speedRatio, float curveRatio);
    void move(bool leftForward, float leftSpeedRatio, bool rightForward, float rightSpeedRatio);
    void stop();

    void setMotorOffsets(float offsetLeft, float offsetRight);

private:
    Timer<T, U>& timer_;
    Motor<T> motorLeft_, motorRight_;

    enum class KickstartDirection : uint8_t {
        FORWARD,
        BACKWARD,
        NONE
    };

    void kickstartMotors(KickstartDirection left, KickstartDirection right);
};

#endif /* _MOVEMENT_MANAGER_H */
