#include "Midi.h"

#include "Timer.h"

Midi::Midi(Pin::Region region, Pin::Id id)
    : MidiPin_(region, id, Pin::Direction::OUT) {}

Midi::~Midi() {
    this->MidiPin_.unset();
}

void Midi::playNote(uint8_t note) {

    
    Timer1& timer = Board::get().getTimer1();

    
        uint32_t frequency = 5;
        uint16_t delay = 5;

        // log("Starting timer for buzzer at frequency: %d\n", frequency);
        timer.setAsPwmFrequency(Timer1::ConfigFrequency::fromFrequency(
            frequency, TimerCompareOutputModeA::CLEAR,
            TimerCompareOutputModeB::CLEAR));
       

}
