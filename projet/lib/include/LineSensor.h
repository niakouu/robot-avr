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
        Pin semi_left;
        Pin center;
        Pin semi_right;
        Pin right;
    };

    LineSensor(Pins linePins);
    ~LineSensor() = default;

    bool isDark(Direction direction) const;

private:
    Pins linePins_;
};

#endif /* _LINE_SENSOR_H */