#ifndef _ROBOT_H
#define _ROBOT_H

#include "Board.h"
#include "Button.h"
#include "DistanceSensor.h"
#include "Led.h" //BidirectionalLed
#include "LineSensor.h"
#include "Midi.h"

class Robot {
public:
    static Robot& get();
    Robot(Robot&) = delete;
    void operator=(const Robot&) = delete;

    const Board& getBoard() const;
    const DistanceSensor& getDistanceSensor() const;
    const BidirectionalLed& getBidirectionalLed() const;
    const LineSensor& getLineSensor() const;
    const Midi& getMidi() const;
    const Button& getButton() const;

    Board& getBoard();
    DistanceSensor& getDistanceSensor();
    BidirectionalLed& getBidirectionalLed();
    LineSensor& getLineSensor();
    Midi& getMidi();
    Button& getButton();

private:
    static Robot robot_;
    Board& board_;
    DistanceSensor distanceSensor_;
    BidirectionalLed bidirectionalLed_;
    LineSensor lineSensor_;
    Midi midi_;
    Button button_;
    Robot() noexcept;
    ~Robot();
};

#endif /* _ROBOT_H */
