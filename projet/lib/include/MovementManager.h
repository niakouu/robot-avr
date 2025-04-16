//
// Travail : tp7
// Section # : 4
// Équipe # : 8290
// Correcteur : Dorine Dantrun
// ----------------------------------------------------------
// Développeurs : Edelina Alieva, Kyle Bouchard, Ilyes Belhaddad et Mohamed
// Elyes Bradai
//
// Description du MovementManager.h :
//        Il contrôle les deux roues et la direction du robot : aller à droite,
//        aller à gauche, aller en avant, et aller en arrière.
//

#ifndef _MOVEMENT_MANAGER_H
#define _MOVEMENT_MANAGER_H

#include "Motor.h"
#include "Timer.h"

enum class KickstartDirection : uint8_t { FORWARD, BACKWARD, NONE };

const static constexpr uint16_t KICKSTART_TIME_MS = 250U;

template <typename T, typename U>
class MovementManager {
public:
    MovementManager(Timer<T, U>& timer, const Motor<T>&& motorLeft,
                    const Motor<T>&& motorRight);
    ~MovementManager();

    void moveForward(float speedRatio);
    void moveBackward(float speedRatio);
    void moveLeft(float speedRatio, float curveRatio);
    void moveRight(float speedRatio, float curveRatio);
    void move(bool leftForward, float leftSpeedRatio, bool rightForward,
              float rightSpeedRatio);
    void stop();

    void setMotorOffsets(float offsetLeft, float offsetRight);

    void kickstartMotors(KickstartDirection left, KickstartDirection right,
                         uint16_t durationMs = KICKSTART_TIME_MS);

private:
    Timer<T, U>& timer_;
    Motor<T> motorLeft_, motorRight_;
};

#endif /* _MOVEMENT_MANAGER_H */
