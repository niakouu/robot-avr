#include "Led.h"

#include <util/delay.h>

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
    _delay_ms(static_cast<uint8_t>(BidirectionalLed::AMBER_CYCLE_TIME_MS / 2));
    this->setColor(BidirectionalLed::Color::GREEN);
    _delay_ms(static_cast<uint8_t>(BidirectionalLed::AMBER_CYCLE_TIME_MS / 2));
}

void BidirectionalLed::executeAmber(uint16_t milliseconds) const {
    for (uint16_t i = 0;
         i < milliseconds / BidirectionalLed::AMBER_CYCLE_TIME_MS; i++) {
        this->executeAmberCycle();
    }
}
