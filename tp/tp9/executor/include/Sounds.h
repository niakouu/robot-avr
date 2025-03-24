#ifndef _SOUNDS_H
#define _SOUNDS_H

#include <stdint.h>

namespace sounds {
    struct MidiNote {
        uint8_t note;
        uint16_t sustainTimeMs;
        uint16_t silenceTimeMs;
    };

    constexpr const MidiNote WINDOWS_BOOT[] = {{75, 300, 0}, {63, 150, 0},
                                               {70, 450, 0}, {68, 600, 0},
                                               {75, 300, 0}, {70, 900, 0}};

    constexpr const MidiNote WINDOWS_SHUTDOWN[] = {{80, 300, 0},
                                                   {75, 300, 0},
                                                   {68, 300, 0},
                                                   {70, 450, 0}};
} // namespace sounds

#endif /* _SOUNDS_H */