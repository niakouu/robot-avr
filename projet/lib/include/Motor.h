//
// Travail : tp7
// Section # : 4
// Équipe # : 8290
// Correcteur : Dorine Dantrun
// ----------------------------------------------------------
// Développeurs : Edelina Alieva, Kyle Bouchard, Ilyes Belhaddad et Mohamed
// Elyes Bradai
//
// Description du Motor.h :
//        Réprésente une roue sur le robot. Quand on utilise la fonction move,
//        il return le bon ratio de vitesse et choisi si le pin direction doit
//        être positive ou négative.
//

#ifndef _MOTOR_H
#define _MOTOR_H

#include "Pin.h"

template <typename T>
class Motor {
public:
    Motor(const Pin&& directionPin, float offset);
    ~Motor();

    /// @brief Spin motor in a direction at a specific speed.
    /// @param speedRatio Speed the motor should spin in, between 0 and 1.
    /// @param forward Whether the motor should spin forwards or backwards.
    /// @return PWM ratio to set for the target speed.
    T move(float speedRatio, bool forward) const;

    void setOffset(float offset);

private:
    Pin directionPin_;
    float offset_;
};

#endif /* _MOTOR_H */