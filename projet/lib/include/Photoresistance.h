//
// Travail : tp7
// Section # : 4
// Équipe # : 8290
// Correcteur : Dorine Dantrun
// ----------------------------------------------------------
// Développeurs : Edelina Alieva, Kyle Bouchard, Ilyes Belhaddad et Mohamed
// Elyes Bradai
//
// Description du Photoresistance.h :
//        Implémente, la logique de capteur de photoresistance et la classe
//        l'intensité de la lumière (LOW, MEDIUM, HIGH) selon des threasholds.
//

#ifndef _PHOTORESISTANCE_H
#define _PHOTORESISTANCE_H

#include "Adc.h"
#include "Pin.h"

class Photoresistance {
public:
    Photoresistance(const Adc& adc, Pin::Id pinId);
    ~Photoresistance() = default;

    enum class Intensity : uint8_t { LOW, MEDIUM, HIGH };

    Intensity readIntensity() const;

private:
    static constexpr uint8_t HIGH_LIGHT_THRESHOLD = 0xE9U;
    static constexpr uint8_t LOW_LIGHT_THRESHOLD = 0xB5U;
    const Adc& adc_;
    Pin::Id pinId_;
};

#endif /* _PHOTORESISTANCE_H */
