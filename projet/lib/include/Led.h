//
// Travail : tp7
// Section # : 4
// Équipe # : 8290
// Correcteur : Dorine Dantrun
// ----------------------------------------------------------
// Développeurs : Edelina Alieva, Kyle Bouchard, Ilyes Belhaddad et Mohamed
// Elyes Bradai
//
// Description du Led.h :
//        Implémente, la logique de la LED sur le board. On peut choisir les
//        ports du board pour la pin positive et la pin négative. Cette classe
//        permet de changer de couleur rouge et vert, et d'executer le
//        changement de ses deux couleurs pour créé la couleur amber.
//

#ifndef _LED_H
#define _LED_H

#include "Pin.h"

class BidirectionalLed {
public:
    enum class Color : uint8_t { RED, GREEN, OFF };
    static constexpr uint8_t AMBER_CYCLE_TIME_MS = 10U;

    /// @brief Creates a bidirectional LED with pin region settings.
    /// @param positiveRegion Positive pin's region.
    /// @param positiveId Positive pin's id.
    /// @param negativeRegion Negative pin's region.
    /// @param negativeId Negative pin's id.
    BidirectionalLed(Pin::Region positiveRegion, Pin::Id positiveId,
                     Pin::Region negativeRegion, Pin::Id negativeId);

    /// @brief Creates a bidirectional LED housed in the same pin region.
    /// @param region Region of positive and negative LED pins.
    /// @param positiveId Positive pin's id.
    /// @param negativeId Negative pin's id.
    BidirectionalLed(Pin::Region region, Pin::Id positiveId,
                     Pin::Id negativeId);

    /// @brief Destroys the LED object, turning it off to save energy.
    ~BidirectionalLed();

    /// @brief Sets the LED's color. See executeAmber() for the amber
    /// color.
    /// @param color Color to set the bidirectional led to.
    void setColor(Color color) const;

    /// @brief Toggles the bidirectional LED to create an amber color. The
    /// duration of this function is AMBER_CYCLE_TIME_MS
    void executeAmberCycle() const;

    /// @brief Toggles the bidirectional LED to create an amber color. The
    /// duration of this function is in the function arguments and is in
    /// milliseconds.
    void executeAmber(uint16_t milliseconds) const;

private:
    Pin positive_, negative_;
};

#endif /* _LED_H */
