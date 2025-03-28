#include "LineSensor.h"

LineSensor::LineSensor(Pins linePins) : linePins_(linePins) {}

bool LineSensor::isDark(Direction direction) const {
    return false;
}