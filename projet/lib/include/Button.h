#ifndef _BUTTON_H
#define _BUTTON_H

#include "Pin.h"

class Button {
public:
    enum class Interrupt : uint8_t { I0, I1, I2 };

    Button(Pin::Region region, Pin::Id id, bool pressedIsHigh);
    Button(Interrupt interrupt, bool pressedIsHigh);
    ~Button();

    bool isPressed() const volatile;
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