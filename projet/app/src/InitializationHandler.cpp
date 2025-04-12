#include "InitializationHandler.h"

#include "Challenge.h"
#include "Robot.h"

bool InitializationHandler::isBPointNorth = false;
bool InitializationHandler::isCPointNorth = false;
static uint8_t cycleCount = 0;
static uint8_t counterInt_ = 0;

InitializationHandler::InitializationHandler()
    : currentState_(InitializationHandler::Point::B), counter_(0) {}

void InitializationHandler::update(uint16_t deltaTimeMs, Challenge& challenge) {

    switch (this->currentState_) {

        case Point::B:
            chooseDirection(InitializationHandler::isBPointNorth);
            break;
        case Point::C:
            chooseDirection(InitializationHandler::isCPointNorth);
            break;
        case Point::EXIT:
            endingPointHandler(challenge);
            break;
        default:
            break;
    }
}

void InitializationHandler::chooseDirection(bool& isPointNorth) {

    Button& extraButton = Robot::get().getExtraButton();
    Button& boardButton = Robot::get().getButton();
    BidirectionalLed& bidirectionalLed = Robot::get().getBidirectionalLed();

    // if (Robot::get().getExtraButton().isEvent() && counter_ == 0) {
    //     printf("CLICKED\n");
    //     Robot::get().getBidirectionalLed().setColor(
    //         BidirectionalLed::Color::RED);
    //     Robot::get().getExtraButton().consumeEvent();
    //     counter_ = 1;
    // }

    // if (Robot::get().getExtraButton().isEvent()
    //     && Robot::get().getExtraButton().isPressed() && counter_ == 1) {
    //     Robot::get().getBidirectionalLed().setColor(
    //         BidirectionalLed::Color::OFF);
    //     printf("NORTH SAVED\n");
    //     isPointNorth = true;
    //     counter_ = 2;
    //     cycleCount++;
    //     Robot::get().getExtraButton().restoreEvent();
    // }

    if (extraButton.isEvent() && cycleCount == 0) {
        if (extraButton.isPressed()) {
            bidirectionalLed.setColor(BidirectionalLed::Color::RED);
            printf("CLICKED\n");
            extraButton.consumeEvent();
            cycleCount++;
        }

        if (!extraButton.isPressed() && cycleCount == 1) {
            bidirectionalLed.setColor(BidirectionalLed::Color::OFF);
            printf("NORTH SAVED\n");
            isPointNorth = true;
            cycleCount++;

            extraButton.consumeEvent();
        }
    }

    if (boardButton.isEvent() && cycleCount == 0) {
        if (boardButton.isPressed()) {
            bidirectionalLed.setColor(BidirectionalLed::Color::RED);
            printf("CLICKED\n");
            boardButton.consumeEvent();
            cycleCount++;
        }

        if (!boardButton.isPressed() && cycleCount == 1) {
            bidirectionalLed.setColor(BidirectionalLed::Color::OFF);
            printf("SOUTH SAVED\n");
            isPointNorth = false;
            cycleCount++;

            boardButton.consumeEvent();
        }
    }

    // if (!Robot::get().getButton().isEvent() && counterInt_ == 0) {
    //     printf("CLICKED\n");
    //     Robot::get().getBidirectionalLed().setColor(
    //         BidirectionalLed::Color::GREEN);
    //     Robot::get().getButton().restoreEvent();
    //     counterInt_ = 1;
    // }
    // if (counterInt_ == 1)
    //     Robot::get().getButton().restoreEvent();

    // if (Robot::get().getButton().isEvent()
    //     && !Robot::get().getButton().isPressed() && counterInt_ == 1) {
    //     Robot::get().getBidirectionalLed().setColor(
    //         BidirectionalLed::Color::OFF);
    //     printf("SOUTH SAVED\n");
    //     isPointNorth = false;
    //     counterInt_ = 2;
    //     cycleCount++;
    //     Robot::get().getButton().restoreEvent(); // ?
    // }

    if (cycleCount == 2) {
        this->currentState_ = InitializationHandler::Point::C;
    } else if (cycleCount == 4) {
        this->currentState_ = InitializationHandler::Point::EXIT;
    }
}

void InitializationHandler::endingPointHandler(Challenge& challenge) {
    Board::get().getWatchdogTimer().sleep(2000, WatchdogTimer::SleepMode::IDLE);
    printf("FINISHED INITIALISATION\n");
    challenge.setState(Challenge::State::FORK_CHALLENGE);
}