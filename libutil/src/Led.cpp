#include "Led.h"

BidirectionalLed::BidirectionalLed(Pin::Region positiveRegion,
                                   Pin::Id positiveId,
                                   Pin::Region negativeRegion,
                                   Pin::Id negativeId)
    : positive_(positiveRegion, Pin::Direction::OUT, positiveId),
      negative_(negativeRegion, Pin::Direction::OUT, negativeId) {}

BidirectionalLed::~BidirectionalLed() {}

void BidirectionalLed::setColor(Color color) {
    switch (color) {
        case BidirectionalLed::Color::OFF:
            this->positive_.unset();
            this->negative_.unset();
            break;
        case BidirectionalLed::Color::GREEN:
            this->positive_.unset();
            this->negative_.set();
            break;
        case BidirectionalLed::Color::RED:
            this->positive_.set();
            this->negative_.unset();
            break;
        default:
            break;
    }
}
