//
// Travail : projet
// Section # : 4
// Équipe # : 8290
// Correcteur : Dorine Dantrun
// ----------------------------------------------------------
// Développeurs : Edelina Alieva, Kyle Bouchard, Ilyes Belhaddad et Mohamed
// Elyes Bradai
//
// Description du Board.h :
//        Détiens toutes les composantes qui font partie du robot. Robot est une
//        classe singleton.
//

#ifndef _ROBOT_H
#define _ROBOT_H

#include "Board.h"
#include "Button.h"
#include "DistanceSensor.h"
#include "Led.h" //BidirectionalLed
#include "LineSensor.h"
#include "Midi.h"
#include "MovementManager.h"

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
    const Button& getExtraButton() const;
    const MovementManager<uint8_t, TimerPrescalerSynchronous>&
    getMovementManager() const;

    Board& getBoard();
    DistanceSensor& getDistanceSensor();
    BidirectionalLed& getBidirectionalLed();
    LineSensor& getLineSensor();
    Midi& getMidi();
    Button& getExtraButton();
    MovementManager<uint8_t, TimerPrescalerSynchronous>& getMovementManager();

    static constexpr float OFFSET_LEFT = 1.0F;
    static constexpr float OFFSET_RIGHT = 1.0F; // 0.94F;

private:
    static Robot robot_;
    Board& board_;
    DistanceSensor distanceSensor_;
    BidirectionalLed bidirectionalLed_;
    LineSensor lineSensor_;
    Midi midi_;
    Button extraButton_;
    MovementManager<uint8_t, TimerPrescalerSynchronous> movementManager_;
    Robot() noexcept;
    ~Robot();
};

#endif /* _ROBOT_H */
