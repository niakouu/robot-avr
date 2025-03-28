#include "DistanceSensor.h"

#include "Board.h"
#include "Pin.h"

DistanceSensor::DistanceSensor(Pin::Id distancePinId)
    : distancePinId_(distancePinId) {}

uint8_t DistanceSensor::getDistanceCm() {
    uint16_t value = Board::get().getAdc().read(static_cast<uint8_t>(this->distancePinId_));

    return value;
}
