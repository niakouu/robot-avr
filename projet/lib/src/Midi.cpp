#include "Midi.h"

#include "Timer.h"
#include "debug.h"

Midi::Midi(Pin::Region region, Pin::Id id)
    : timer_(Board::get().getTimer1()), pin_(region, id, Pin::Direction::OUT) {
    this->pin_.unset();
}

static constexpr const uint8_t FIRST_NOTE = 45;
static constexpr const float FREQUENCIES[] = {
    110.0F,  116.54F,
    123.47F, 130.81F,
    138.59F, 146.83F,
    155.56F, 164.81F,
    174.61F, 185.00F,
    196.00F, 207.65F,
    220.0F,  233.08F,
    246.94F, 261.63F,
    277.18F, 293.66F,
    311.13F, 329.63F,
    349.23F, 369.99F,
    392.00F, 415.30F,
    440.0F,  466.16F,
    493.88F, 523.25F,
    554.37F, 587.33F,
    622.25F, 659.26F,
    698.46F, 739.99F,
    783.99F, 830.61F,
    880.0F};

void Midi::playNote(uint8_t note) const {
    note -= FIRST_NOTE;
    if (sizeof(FREQUENCIES) / sizeof(*FREQUENCIES) <= note)
        return;

    const float frequency = FREQUENCIES[note];
    this->timer_.setAsPwmFrequency(
        Timer1::ConfigFrequency::fromFrequency(frequency,
                                               TimerCompareOutputModeA::TOGGLE,
                                               TimerCompareOutputModeB::TOGGLE));

    this->timer_.start();
}

void Midi::stop() const {
    this->timer_.stop();
}