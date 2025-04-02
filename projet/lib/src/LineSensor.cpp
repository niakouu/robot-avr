#include "LineSensor.h"

LineSensor::LineSensor(Pins linePins) : linePins_(linePins) {
    linePins_.left.updateDirection(Pin::Direction::IN);
    linePins_.semiLeft.updateDirection(Pin::Direction::IN);
    linePins_.center.updateDirection(Pin::Direction::IN);
    linePins_.semiRight.updateDirection(Pin::Direction::IN);
    linePins_.right.updateDirection(Pin::Direction::IN);
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

LineSensor::Readings LineSensor::getReadings() const {
    return {
        .isLeftDark = linePins_.left.read(),
        .isSemiLeftDark = linePins_.semiLeft.read(),
        .isCenterDark = linePins_.center.read(),
        .isSemiRightDark = linePins_.semiRight.read(),
        .isRightDark = linePins_.right.read(),
    };
}

int8_t LineSensor::Readings::getAverage() const {
    int8_t average = 0;
    int8_t members = 0;

    if (this->isLeftDark) {
        average -= 4;
        ++members;
    }

    if (this->isSemiLeftDark) {
        average -= 2;
        ++members;
    }

    if (this->isCenterDark) {
        ++members;
    }

    if (this->isSemiRightDark) {
        average += 2;
        ++members;
    }

    if (this->isRightDark) {
        average += 4;
        ++members;
    }

    return members == 0 ? 0 : average / members;
}

uint8_t LineSensor::Readings::getDarkLineCount() const {
    return this->isLeftDark + this->isSemiLeftDark + this->isCenterDark + this->isSemiRightDark + this->isRightDark;
}