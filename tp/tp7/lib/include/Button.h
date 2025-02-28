#ifndef _BUTTON_H
#define _BUTTON_H

#include "Pin.h"

class Button {
public:
    Button(Pin::Region region, Pin::Id id, bool pressedIsHigh);
    ~Button();

    bool isPressed() const;
    bool isEvent() const;
    void consumeEvent();
    void setPressed();

private:
    bool isPressed_, eventConsumed_, pressedIsHigh_;
};

#endif /* _BUTTON_H */