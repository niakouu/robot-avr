#ifndef _MIDI_H
#define _MIDI_H

#include "Pin.h"
#include "Board.h"
#include "Timer.h"

class Midi {
public:
    Midi(Pin::Region region, Pin::Id id);
    ~Midi();

    void playNote(uint8_t note);

private:
    Pin MidiPin_;
    // Timer1& timer_;
};

#endif /* _MIDI_H */