#ifndef _DISTANCE_SENSOR_H
#define _DISTANCE_SENSOR_H

#include "Pin.h"

class DistanceSensor {
public:
    DistanceSensor(Pin::Id distancePinId);
    ~DistanceSensor() = default;

    uint8_t getDistanceCm() const;

private:
    static constexpr uint8_t MIN_DISTANCE = 10U;
    static constexpr uint8_t MAX_DISTANCE = 80U;
    static constexpr float MAX_VOLTAGE = 2.25F;
    uint8_t distancePinId_;

    static uint8_t calculateDistanceFromVoltage(float voltageIn);
};

#endif /* _DISTANCE_SENSOR_H */