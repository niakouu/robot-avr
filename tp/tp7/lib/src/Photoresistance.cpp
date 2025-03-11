#include "Photoresistance.h"

Photoresistance::Photoresistance(const Adc& adc, Pin::Id pinId)
    : adc_(adc), pinId_(pinId) {}

Photoresistance::~Photoresistance() {}

Photoresistance::Intensity Photoresistance::readIntensity() const {
    uint8_t const data =
        static_cast<uint8_t>(adc_.read(static_cast<uint8_t>(pinId_)) >> 2);
    if (data >= HIGH_LIGHT_THRESHOLD) {
        return Intensity::HIGH;
    } else if (data < LOW_LIGHT_THRESHOLD) {
        return Intensity::LOW;
    } else {
        return Intensity::MEDIUM;
    }
}
