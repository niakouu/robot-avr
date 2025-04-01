#include "LineSensor.h"

LineSensor::LineSensor(Pins linePins) : linePins_(linePins) {
    linePins_.left.updateDirection(Pin::Direction::OUT);
    linePins_.semiLeft.updateDirection(Pin::Direction::OUT);
    linePins_.center.updateDirection(Pin::Direction::OUT);
    linePins_.semiRight.updateDirection(Pin::Direction::OUT);
    linePins_.right.updateDirection(Pin::Direction::OUT);
}

bool LineSensor::isDark(Direction direction) const {
    switch (direction) {
        case Direction::LEFT:
            return linePins_.left.read();
        case Direction::SEMI_LEFT:
            return linePins_.semiLeft.read();  
        case Direction::CENTER:
            return linePins_.center.read();  
        case Direction::SEMI_RIGHT:
            return linePins_.semiRight.read();  
        case Direction::RIGHT:
            return linePins_.right.read();  
        default:
            return false;
    }
}