#ifndef _LED_H
#define _LED_H

#include "Pin.h"

class BidirectionalLed {
public:
    enum class Color { RED, GREEN, OFF };

    BidirectionalLed(Pin::Region positiveRegion, Pin::Id positiveId,
                     Pin::Region negativeRegion, Pin::Id negativeId);
    ~BidirectionalLed();

    void setColor(Color color);

private:
    Pin positive_, negative_;
};

#endif /* _LED_H */