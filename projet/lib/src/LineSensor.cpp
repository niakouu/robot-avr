#include "LineSensor.h"

LineSensor::LineSensor(Pins linePins) : linePins_(linePins) {}

bool LineSensor::isDark(Direction direction) const {
    switch (direction) {
        case Direction::LEFT:
            return linePins_.left.read();
        case Direction::SEMI_LEFT:
            return linePins_.semi_left.read();  
        case Direction::CENTER:
            return linePins_.center.read();  
        case Direction::SEMI_RIGHT:
            return linePins_.semi_right.read();  
        case Direction::RIGHT:
            return linePins_.right.read();  
        default:
            return false;
    }
}