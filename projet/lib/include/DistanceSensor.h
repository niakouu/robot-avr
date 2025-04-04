#ifndef _DISTANCE_SENSOR_H
#define _DISTANCE_SENSOR_H

#include "Pin.h"

class DistanceSensor {
public:
    uint8_t getDistanceCm() const;

protected:
    DistanceSensor(Pin::Id distancePinId);
    ~DistanceSensor() = default;

    friend class Robot;

private:
    static constexpr uint8_t MIN_DISTANCE = 10U;
    static constexpr uint8_t MAX_DISTANCE = 80U;
    static constexpr float MAX_VOLTAGE = 2.25F;
    uint8_t distancePinId_;

    static uint8_t calculateDistanceFromVoltage(float voltageIn);
};

#endif /* _DISTANCE_SENSOR_H */