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

    if (!Robot::get().getExtraButton().isEvent() && counter_ == 0) {
        printf("CLICKED\n");
        Robot::get().getBidirectionalLed().setColor(
            BidirectionalLed::Color::RED);
        Robot::get().getExtraButton().restoreEvent();
        counter_ = 1;
    }
    if (counter_ == 1)
    Robot::get().getExtraButton().restoreEvent();
        
    if (Robot::get().getExtraButton().isEvent()
        && Robot::get().getExtraButton().isPressed()
        && counter_ == 1) {
        Robot::get().getBidirectionalLed().setColor(
            BidirectionalLed::Color::OFF);
        printf("NORTH SAVED\n");
        isPointNorth = true;
        counter_ = 2;
        cycleCount++;
        Robot::get().getExtraButton().restoreEvent(); // ?
    }


    if (!Robot::get().getButton().isEvent() && counter_ == 0) {
        printf("CLICKED\n");
        Robot::get().getBidirectionalLed().setColor(
            BidirectionalLed::Color::GREEN);
        Robot::get().getButton().restoreEvent();
        counter_ = 1;
    }
    if (counter_ == 1)
    Robot::get().getButton().restoreEvent();
        
    if (Robot::get().getButton().isEvent()
        && Robot::get().getButton().isPressed()
        && counter_ == 1) {
        Robot::get().getBidirectionalLed().setColor(
            BidirectionalLed::Color::OFF);
        printf("SOUTH SAVED\n");
        isPointNorth = false;
        counter_ = 2;
        cycleCount++;
        Robot::get().getButton().restoreEvent(); // ?
    }

    // printf("i %d\n", counter_);


    // if (!Robot::get().getButton().isEvent()) {
    //     printf("CLICKED\n");
    //     Robot::get().getBidirectionalLed().setColor(
    //         BidirectionalLed::Color::GREEN);
    //     Robot::get().getButton().restoreEvent();
    // }
    // if (Robot::get().getButton().isEvent()
    //     && Robot::get().getButton().isPressed()) {
    //     Robot::get().getBidirectionalLed().setColor(
    //         BidirectionalLed::Color::OFF);
    //     printf("SOUTH SAVED\n");
    //     isPointNorth = false;
    //     counter_++;
    // }

    if (cycleCount == 2) {
        counter_ = 0;
        this->currentState_ = InitializationHandler::Point::C;
    } else if (cycleCount == 3) {
        this->currentState_ = InitializationHandler::Point::EXIT;
    }
}

void InitializationHandler::endingPointHandler(Challenge& challenge) {
    Board::get().getWatchdogTimer().sleep(2000, WatchdogTimer::SleepMode::IDLE);
    printf("FINISH\n");
    challenge.setState(Challenge::State::LOCATE);
}