#include "Emulator.h"

#include "Board.h"
#include "Motor.h"
#include "MovementManager.h"
#include "Pin.h"
#include "Timer.h"

Emulator::Emulator()
    : instructionPointer_(0x0), returnAddress_(0x0), cycleCount_(0),
      isDone_(false),
      movementManager_(Board::get().getTimer2(),
                       Motor<uint8_t>(Pin(Pin::Region::D, Pin::Id::P5), 0.0F),
                       Motor<uint8_t>(Pin(Pin::Region::D, Pin::Id::P4), 0.0F)),
      bidirectionalLed_(Pin::Region::B, Pin::Id::P5, Pin::Id::P4) {}

void Emulator::executeNextInstruction(uint16_t data) {
    Emulator::Instruction instruction = static_cast<Emulator::Instruction>(data >> UINT8_WITDH);
    uint8_t operand = static_cast<uint8_t>(data & UINT8_MAX);
    switch (instruction)
    {
    case Emulator::Instruction::START:
        this->isDone_ = false;
        break;
    case Emulator::Instruction::WAIT:
    
        break;
    case Emulator::Instruction::TURN_ON_LED:
    
        break;
    case Emulator::Instruction::TURN_OFF_LED:
    
        break;
    case Emulator::Instruction::PLAY_SOUND:
    
        break;
    case Emulator::Instruction::STOP_SOUND:
    
        break;
    case Emulator::Instruction::STOP_MOTOR_0:
    case Emulator::Instruction::STOP_MOTOR_1:
    
        break;
    case Emulator::Instruction::MOVE_FORWARD:
        break;
    case Emulator::Instruction::MOVE_BACKWARD:
        break;
    case Emulator::Instruction::TURN_RIGHT:
        break;
    case Emulator::Instruction::TURN_LEFT:
        break;
    case Emulator::Instruction::START_LOOP:
        break;
    case Emulator::Instruction::END_LOOP:
        break;
    case Emulator::Instruction::END:
        break;
    default:
        break;
    }
}

uint16_t Emulator::getInstructionPointer() const {
    return this->instructionPointer_;
}

bool Emulator::getIsDone() const {
    return this->isDone_;
}
