#ifndef _MOTOR_H
#define _MOTOR_H

#include "Pin.h"

template <typename T>
class Motor {
public:
    Motor(const Pin&& directionPin, float offset);
    ~Motor();

    /// @brief Spin motor in a direction at a specific speed.
    /// @param speedRatio Speed the motor should spin in, between 0 and 1.
    /// @param forward Whether the motor should spin forwards or backwards.
    /// @return PWM ratio to set for the target speed.
    T move(float speedRatio, float curveRatio, bool forward) const;

    void setOffset(float offset);

private:
    Pin directionPin_;
    float offset_;
};

#endif /* _MOTOR_H */