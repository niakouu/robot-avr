#ifndef _BUTTON_H
#define _BUTTON_H

#include "Pin.h"

class Button {
public:
    Button(Pin::Region region, Pin::Id id, bool pressedIsHigh);
    ~Button() = default;

    bool isPressed();
    bool isEvent() const;
    void consumeEvent();
    void restoreEvent();
    void setPressed();

private:
    bool isPressed_, eventConsumed_, pressedIsHigh_;
    Pin buttonPin_;
};

#endif /* _BUTTON_H */