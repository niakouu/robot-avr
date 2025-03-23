#ifndef _EMULATOR_H
#define _EMULATOR_H

#include "Board.h"
#include "MovementManager.h"
#include "Led.h"
#include "Pin.h"
#include "Midi.h"

class Emulator {
public:
    Emulator();
    ~Emulator() = default;

    void executeNextInstruction(uint16_t data);
    uint16_t getInstructionPointer() const;
    bool isDone() const;

private:
    enum class State : uint8_t {
        NOT_STARTED,
        RUNNING,
        DONE
    };

    enum class Instruction : uint8_t {
        START = 0x01,
        WAIT = 0x02,
        TURN_ON_LED = 0x44,
        TURN_OFF_LED = 0x45,
        PLAY_SOUND = 0x48,
        STOP_SOUND = 0x09,
        STOP_MOTOR_0 = 0x60,
        STOP_MOTOR_1 = 0x61,
        MOVE_FORWARD = 0x62,
        MOVE_BACKWARD = 0x63,
        TURN_RIGHT = 0x64,
        TURN_LEFT = 0x65,
        START_LOOP = 0xC0,
        END_LOOP = 0xC1,
        END = 0xFF
    };

    uint16_t instructionPointer_;
    uint16_t returnAddress_;
    uint16_t cycleCount_;
    State state_;
    MovementManager<uint8_t, TimerPrescalerSynchronous> movementManager_;
    BidirectionalLed bidirectionalLed_;
    Midi midi_;
};

#endif /* _EMULATOR_H */