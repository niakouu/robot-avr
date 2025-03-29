#include "DistanceSensor.h"

#include "Board.h"
#include "Pin.h"
#include "debug.h"
#define DEBUG

DistanceSensor::DistanceSensor(Pin::Id distancePinId)
    : distancePinId_(static_cast<uint8_t>(distancePinId)) {}

uint8_t DistanceSensor::getDistanceCm() const {
    const float adc = static_cast<float>(Board::get().getAdc().read(this->distancePinId_));
    const float voltageIn = (adc * Adc::VOLTAGE_REFERENCE) / static_cast<float>(Adc::MAX_ADC_VALUE);

    const uint8_t distance = calculateDistanceFromVoltage(voltageIn);

    if (distance < MIN_DISTANCE || voltageIn > MAX_VOLTAGE)
        return MIN_DISTANCE;

    if (distance > MAX_DISTANCE)
        return MAX_DISTANCE;
    
    return distance;
}

uint8_t DistanceSensor::calculateDistanceFromVoltage(float voltageIn) {
    return static_cast<uint8_t>((36.0f * voltageIn * voltageIn) - (129.0f * voltageIn) + 120.0f);
}
