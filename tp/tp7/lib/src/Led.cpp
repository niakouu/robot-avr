#include <util/delay.h>
#include "Led.h"

constexpr uint8_t DELAY_AMBER_CYCLE_MS = 5;

BidirectionalLed::BidirectionalLed(Pin::Region positiveRegion,
                                   Pin::Id positiveId,
                                   Pin::Region negativeRegion,
                                   Pin::Id negativeId)
    : positive_(positiveRegion, positiveId, Pin::Direction::OUT),
      negative_(negativeRegion, negativeId, Pin::Direction::OUT) {}

BidirectionalLed::BidirectionalLed(Pin::Region region, Pin::Id positiveId,
                                   Pin::Id negativeId)
    : BidirectionalLed(region, positiveId, region, negativeId) {}

BidirectionalLed::~BidirectionalLed() {
    this->setColor(Color::OFF);
}

void BidirectionalLed::setColor(Color color) const {
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

void BidirectionalLed::executeAmberCycle() const {
    this->setColor(BidirectionalLed::Color::RED);
    _delay_ms(DELAY_AMBER_CYCLE_MS);
    this->setColor(BidirectionalLed::Color::GREEN);
    _delay_ms(DELAY_AMBER_CYCLE_MS);
}