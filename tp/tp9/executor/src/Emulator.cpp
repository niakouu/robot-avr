#include "Emulator.h"

#include "Board.h"
#include "Midi.h"
#include "Motor.h"
#include "MovementManager.h"
#include "Pin.h"
#include "Timer.h"
#include "WatchdogTimer.h"
#include "common.h"
#include "debug.h"

Emulator::Emulator()
    : instructionPointer_(0x0), returnAddress_(0x0), cycleCount_(0),
      state_(Emulator::State::NOT_STARTED),
      movementManager_(Board::get().getTimer2(),
                       Motor<uint8_t>(Pin(Pin::Region::D, Pin::Id::P5), 0.0F),
                       Motor<uint8_t>(Pin(Pin::Region::D, Pin::Id::P4), 0.0F)),
      bidirectionalLed_(Pin::Region::B, Pin::Id::P5, Pin::Id::P4),
      midi_(Pin::Region::D, Pin::Id::P3) {}

void Emulator::executeNextInstruction(uint16_t data) {
    const Emulator::Instruction instruction =
        static_cast<Emulator::Instruction>(data >> UINT8_WIDTH);

    const uint8_t operand = static_cast<uint8_t>(data & UINT8_MAX);

    if (this->state_ == Emulator::State::DONE)
        return;

    this->instructionPointer_ += sizeof(uint16_t);

    if (this->state_ == Emulator::State::NOT_STARTED) {
        if (instruction == Emulator::Instruction::START)
            this->state_ = Emulator::State::RUNNING;

        return;
    }

    switch (instruction) {
        case Emulator::Instruction::WAIT:
            Board::get().getWatchdogTimer().sleep(
                WAIT_TIME_MS * operand, WatchdogTimer::SleepMode::IDLE);
            break;
        case Emulator::Instruction::TURN_ON_LED:
            if (operand == 1)
                bidirectionalLed_.setColor(BidirectionalLed::Color::GREEN);
            else if (operand == 2)
                bidirectionalLed_.setColor(BidirectionalLed::Color::RED);
            break;
        case Emulator::Instruction::TURN_OFF_LED:
            bidirectionalLed_.setColor(BidirectionalLed::Color::OFF);
            break;
        case Emulator::Instruction::PLAY_SOUND:
            this->midi_.playNote(operand);
            break;
        case Emulator::Instruction::STOP_SOUND:
            this->midi_.stop();
            break;
        case Emulator::Instruction::STOP_MOTOR_0:
        case Emulator::Instruction::STOP_MOTOR_1:
            movementManager_.stop();
            break;
        case Emulator::Instruction::MOVE_FORWARD:
            movementManager_.moveForward(static_cast<float>(operand)
                                         / UINT8_MAX);
            break;
        case Emulator::Instruction::MOVE_BACKWARD:
            movementManager_.moveBackward(static_cast<float>(operand)
                                          / UINT8_MAX);
            break;
        case Emulator::Instruction::TURN_RIGHT:
            movementManager_.moveRight(1.0F, 0.0F);
            break;
        case Emulator::Instruction::TURN_LEFT:
            movementManager_.moveLeft(1.0F, 0.0F);
            break;
        case Emulator::Instruction::START_LOOP:
            this->cycleCount_ = operand;
            this->returnAddress_ = this->instructionPointer_;
            break;
        case Emulator::Instruction::END_LOOP:
            if (this->cycleCount_ != 0) {
                this->cycleCount_--;
                this->instructionPointer_ = this->returnAddress_;
            }
            break;
        case Emulator::Instruction::END:
            this->movementManager_.stop();
            this->bidirectionalLed_.setColor(BidirectionalLed::Color::OFF);
            this->state_ = State::DONE;
            // TODO: Add sound.
            break;
        default:
            break;
    }
}

uint16_t Emulator::getInstructionPointer() const {
    return this->instructionPointer_;
}

bool Emulator::isDone() const {
    return this->state_ == Emulator::State::DONE;
}
