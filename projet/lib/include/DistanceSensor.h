#ifndef _DISTANCE_SENSOR_H
#define _DISTANCE_SENSOR_H

#include "Pin.h"

class DistanceSensor {
public:
    DistanceSensor();
    ~DistanceSensor() = default;

    uint8_t getDistanceCm();

private:
    Pin distancePin_;
};

#endif /* _DISTANCE_SENSOR_H */