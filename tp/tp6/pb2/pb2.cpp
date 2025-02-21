//
// Travail : TP2
// Section # : 4
// Équipe # : 82
// Correcteur : CORRECTEUR
// ----------------------------------------------------------
// Développeurs : Edelina Alieva, Kyle Bouchard
// Description du pb2.cpp : Une DEL bicolore change de couleur dependemment de
//                          l'intensité lumineuse captée par le robot.
// Identification matérielles : Photorésistance connectée aux CAN sur la broche
//                              A1.
//                              DEL bidirectionelle sur la carte mère:
//                                  - DEL côté positive connecté à la broche B2
//                                  (sortie)
//                                  - DEL côté négative connecté à la broche B1
//                                  (sortie)
#define F_CPU 8'000'000UL
#include <avr/wdt.h>

#include "Adc.h"
#include "Led.h"
#include "sleep.h"

constexpr uint8_t HIGH_LIGHT_THRESHOLD = 0xE6;
constexpr uint8_t LOW_LIGHT_THRESHOLD = 0xA3;

int main() {
    Adc adc;
    BidirectionalLed bidirectionalLed{Pin::Region::B, Pin::Id::P1, Pin::Id::P0};

    uint8_t data;
    while (true) {
        data = static_cast<uint8_t>(adc.read(0) >> 2);

        if (data >= HIGH_LIGHT_THRESHOLD) {
            bidirectionalLed.setColor(BidirectionalLed::Color::RED);
            rawSleep(WDTO_15MS, SLEEP_MODE_IDLE);
        } else if (data < LOW_LIGHT_THRESHOLD) {
            bidirectionalLed.setColor(BidirectionalLed::Color::GREEN);
            rawSleep(WDTO_15MS, SLEEP_MODE_IDLE);
        } else
            bidirectionalLed.executeAmberCycle();
    }

    return 0;
}