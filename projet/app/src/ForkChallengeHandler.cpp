// #include "ForkChallengeHandler.h"

// #include "Challenge.h"
// #include "Robot.h"
// #include "InitializationHandler.h"

// ForkChallengeHandler::ForkChallengeHandler()
//     : currentState_(ForkChallengeHandler::Point::B), counter_(0) {}

// void ForkChallengeHandler::update(uint16_t deltaTimeMs, Challenge& challenge) {
//     LineFollower<uint8_t, TimerPrescalerSynchronous>& lineFollower =
//         challenge.getLineFollower();

//     if (!lineFollower.isLost()) {
//         return;
//     }

//     switch (this->currentState_) {
//         case ForkChallengeHandler::Point::B:
//             goNextPoint(InitializationHandler::isBPointNorth, lineFollower, challenge);
//             break;
//         case ForkChallengeHandler::Point::C:
//         goNextPoint(InitializationHandler::isCPointNorth, lineFollower, challenge);
//             break;
//         case ForkChallengeHandler::Point::EXIT:
//             endingPointHandler(challenge, lineFollower);
//             break;
//         default:
//             break;
//     }
// }


// void ForkChallengeHandler::goNextPoint(bool& isPointNorth, LineFollower<uint8_t, TimerPrescalerSynchronous>& lineFollower, Challenge& challenge){

//     Robot& robot = Robot::get();
//     int rotateTime = 250;

//     if(isPointNorth){
//         robot.getMovementManager().moveLeft(Challenge::SPEED, 0.0F);
//         Board::get().getWatchdogTimer().sleep(rotateTime, WatchdogTimer::SleepMode::IDLE);
//         robot.getMovementManager().moveForward(Challenge::SPEED);
//         Board::get().getWatchdogTimer().sleep(rotateTime, WatchdogTimer::SleepMode::IDLE);
//     }else{
//         robot.getMovementManager().moveRight(Challenge::SPEED, 0.0F);
//         Board::get().getWatchdogTimer().sleep(rotateTime, WatchdogTimer::SleepMode::IDLE);
//         robot.getMovementManager().moveForward(Challenge::SPEED);
//         Board::get().getWatchdogTimer().sleep(rotateTime, WatchdogTimer::SleepMode::IDLE);
//     }

//     lineFollower.start();
// }



// void ForkChallengeHandler::endingPointHandler(
//     Challenge& challenge,
//     LineFollower<uint8_t, TimerPrescalerSynchronous>& lineFollower) {
//     challenge.setState(Challenge::State::FOLLOW_LINE);
//     lineFollower.start();
// }