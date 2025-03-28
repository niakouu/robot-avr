#ifndef _DISTANCE_SENSOR_H
#define _DISTANCE_SENSOR_H

#include "Pin.h"

class DistanceSensor {
public:
    DistanceSensor(Pin::Id distancePinId);
    ~DistanceSensor() = default;

    uint8_t getDistanceCm();

private:
    Pin::Id distancePinId_;
};

#endif /* _DISTANCE_SENSOR_H */