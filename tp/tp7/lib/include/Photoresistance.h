#ifndef _PHOTORESISTANCE_H
#define _PHOTORESISTANCE_H

#include "Adc.h"
#include "Pin.h"

class Photoresistance {
public:
    Photoresistance(const Adc& adc, Pin::Id pinId);
    ~Photoresistance();

    enum class Intensity : uint8_t { LOW, MEDIUM, HIGH };

    Intensity readIntensity() const;

private:
    static constexpr uint8_t HIGH_LIGHT_THRESHOLD = 0xE9;
    static constexpr uint8_t LOW_LIGHT_THRESHOLD = 0xB5;
    const Adc& adc_;
    Pin::Id pinId_;
};

#endif /* _PHOTORESISTANCE_H */
