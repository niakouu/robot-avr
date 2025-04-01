#ifndef _LINE_SENSOR_H
#define _LINE_SENSOR_H

#include "Pin.h"

class LineSensor {
public:
    enum class Direction : uint8_t {
        LEFT = 0U,
        SEMI_LEFT = 1U,
        CENTER = 2U,
        SEMI_RIGHT = 3U,
        RIGHT = 4U
    };

    struct Pins {
        Pin left;
        Pin semiLeft;
        Pin center;
        Pin semiRight;
        Pin right;
    };

    struct Readings {
        bool isLeftDark : 1;
        bool isSemiLeftDark : 1;
        bool isCenterDark : 1;
        bool isSemiRightDark : 1;
        bool isRightDark : 1;
    };

    LineSensor(Pins linePins);
    ~LineSensor() = default;

    bool isDark(Direction direction) const;
    Readings getReadings() const;
private:
    Pins linePins_;
};

#endif /* _LINE_SENSOR_H */