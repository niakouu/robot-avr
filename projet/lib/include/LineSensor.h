#ifndef _LINE_SENSOR_H
#define _LINE_SENSOR_H

#include "Pin.h"

class LineSensor {
public:
    enum class Direction : uint8_t {
        LEFT,
        SEMI_LEFT,
        CENTER,
        SEMI_RIGHT,
        RIGHT
    };

    LineSensor();
    ~LineSensor() = default;

    bool isDark(Direction direction) const;

private:
    static constexpr uint8_t INPUT_NUMBER = 5;

    Pin linePins_[INPUT_NUMBER];
};

#endif /* _LINE_SENSOR_H */