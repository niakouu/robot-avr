#include "InitializationHandler.h"

#include "Challenge.h"
#include "Robot.h"

bool InitializationHandler::isBPointNorth = false;
bool InitializationHandler::isCPointNorth = false;
static uint8_t cycleCount = 1;


InitializationHandler::InitializationHandler()
    : currentState_(InitializationHandler::Point::B), counter_(0) {}

void InitializationHandler::update(uint16_t deltaTimeMs, Challenge& challenge) {

    switch (this->currentState_) {
        case InitializationHandler::Point::B: {
            chooseDirection(InitializationHandler::isBPointNorth);
            break;
        }
        case InitializationHandler::Point::C: {
            chooseDirection(InitializationHandler::isCPointNorth);
            break;
        }
        case InitializationHandler::Point::EXIT: {
            endingPointHandler(challenge);
            break;
        }
    }
}

void InitializationHandler::chooseDirection(bool& isPointNorth) {

    // led.setColor(BidirectionalLed::Color::GREEN);
    // Robot::get().getExtraButton().consumeEvent();
    // if (Robot::get().getExtraButton().isPressed()) {
    //     Robot::get().getBidirectionalLed().setColor(BidirectionalLed::Color::RED);
    //     printf("rouge");
    // }
    // // Robot::get().getExtraButton().rest();
    
    if (!Robot::get().getExtraButton().isEvent()) {
        printf("CLICKED\n");
        Robot::get().getBidirectionalLed().setColor(
            BidirectionalLed::Color::RED);
        Robot::get().getExtraButton().restoreEvent();
    }
    if (Robot::get().getExtraButton().isEvent()
        && Robot::get().getExtraButton().isPressed()
        && counter_ < cycleCount) {
        Robot::get().getBidirectionalLed().setColor(
            BidirectionalLed::Color::OFF);
        printf("NORTH SAVED\n");
        isPointNorth = true;
        counter_++;
        Robot::get().getExtraButton().restoreEvent(); // ?
    }

    if (!Robot::get().getButton().isEvent()) {
        printf("CLICKED\n");
        Robot::get().getBidirectionalLed().setColor(
            BidirectionalLed::Color::GREEN);
        Robot::get().getButton().restoreEvent();
    }
    if (Robot::get().getButton().isEvent()
        && Robot::get().getButton().isPressed()) {
        Robot::get().getBidirectionalLed().setColor(
            BidirectionalLed::Color::OFF);
        printf("SOUTH SAVED\n");
        isPointNorth = false;
        counter_++;
    }
    

    if (this->counter_ == 1) {
        this->currentState_ = InitializationHandler::Point::C;
        cycleCount++;
    } else if (this->counter_ == 2) {
        this->currentState_ = InitializationHandler::Point::EXIT;
    }

    // led.setColor(BidirectionalLed::Color::OFF);
}

void InitializationHandler::endingPointHandler(Challenge& challenge) {
    Board::get().getWatchdogTimer().sleep(2000, WatchdogTimer::SleepMode::IDLE);
    printf("FINISH\n");
    challenge.setState(Challenge::State::LOCATE);
}