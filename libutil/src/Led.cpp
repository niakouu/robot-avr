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
            this->positive_.write(0);
            this->negative_.write(0);
            break;
        case BidirectionalLed::Color::GREEN:
            this->positive_.write(0);
            this->negative_.write(1);
            break;
        case BidirectionalLed::Color::RED:
            this->positive_.write(1);
            this->negative_.write(0);
            break;
        default:
            break;
    }
}
