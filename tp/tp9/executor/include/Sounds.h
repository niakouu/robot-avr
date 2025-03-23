#ifndef _SOUNDS_H
#define _SOUNDS_H

#include <stdint.h>

namespace sounds {
    struct MidiNote {
        uint8_t note;
        uint8_t sustainTimeMs;
        uint8_t silenceTimeMs;
    };

    constexpr const MidiNote WINDOWS_BOOT[] = {{75, 12, 0}, {63, 6, 0},
                                               {70, 18, 0}, {68, 24, 0},
                                               {75, 12, 0}, {70, 36, 0}};

    constexpr const MidiNote WINDOWS_SHUTDOWN[] = {{80, 12, 0},
                                                   {75, 12, 0},
                                                   {68, 18, 0},
                                                   {70, 18, 0}};
} // namespace sounds

#endif /* _SOUNDS_H */