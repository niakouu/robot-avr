// 
// Travail : tp7
// Section # : 4
// Équipe # : 8290
// Correcteur : Dorine Dantrun
// ----------------------------------------------------------
// Développeurs : Edelina Alieva, Kyle Bouchard, Ilyes Belhaddad et Mohamed
// Elyes Bradai
//
// Description du Button.h :
//        Implémente, la logique des buttons soit qu'il soit le button-poussoir ou l'interrupteur. 
//

#ifndef _BUTTON_H
#define _BUTTON_H

#include "Pin.h"

class Button {
public:
    enum class Interrupt : uint8_t { I0, I1, I2 };

    Button(Pin::Region region, Pin::Id id, bool pressedIsHigh);
    Button(Interrupt interrupt, bool pressedIsHigh);
    ~Button();

    /// @brief Determines if the button is currently pressed or not.
    /// @return True if button is pressed, false otherwise.
    bool isPressed() const volatile;

    /// @brief Determines if there was a press or a realese of button.
    /// @return True if event, false otherwise.
    bool isEvent() const volatile;

    void consumeEvent() volatile;
    void restoreEvent() volatile;
    void setPressed() volatile;

private:
    bool pressedIsHigh_;
    volatile bool isPressed_, eventConsumed_;
    Pin buttonPin_;

    struct InterruptMappings {
        uint8_t senseControl[2];
        uint8_t interruptMask;
    };

    struct {
        bool enabled_;
        InterruptMappings mappings_;
    } interrupts_;
};

#endif /* _BUTTON_H */