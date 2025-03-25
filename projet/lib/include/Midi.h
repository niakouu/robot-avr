#ifndef _MIDI_H
#define _MIDI_H

#include "Pin.h"
#include "Board.h"
#include "Timer.h"

class Midi {
public:
    Midi(Pin::Region region, Pin::Id id);
    ~Midi() = default;

    void playNote(uint8_t note) const;
    void stop() const;

private:
    Timer1& timer_;
    Pin pin_;
};

#endif /* _MIDI_H */