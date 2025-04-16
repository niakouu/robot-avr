#ifndef _ORCHESTRATOR_H
#define _ORCHESTRATOR_H

#include "Midi.h"
#include "Music.h"
#include <stddef.h>

class Orchestrator {
public:
    static Orchestrator& get();
    Orchestrator(Orchestrator&) = delete;
    void operator=(const Orchestrator&) = delete;

    void loadSong(const music::MidiNote *song, size_t songLength);
    void update(uint16_t deltaTimeMs);
    void play();
    void pause();
    bool isPaused() const;

private:
    static Orchestrator orchestrator_;
    explicit Orchestrator(Midi& midiPlayer);

    bool isPaused_;
    const music::MidiNote *song_;
    uint16_t sustainTimeLeft_, silenceTimeLeft_;
    size_t songLength_, songIndex_;
    Midi& midiPlayer_;
};

#endif /* _ORCHESTRATOR_H */