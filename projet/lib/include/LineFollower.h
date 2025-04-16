//
// Travail : projet
// Section # : 4
// Équipe # : 8290
// Correcteur : Dorine Dantrun
// ----------------------------------------------------------
// Développeurs : Edelina Alieva, Kyle Bouchard, Ilyes Belhaddad et Mohamed
// Elyes Bradai
//
// Description du LineFollower.h :
//        La classe permet de suivre la ligne noire. Elle permet de tourner,
//        aller droit ou stop le robot. La ligne check sa prochain action à
//        chaque update.
//
// Table des états :
//
// Table des états :
// ---------------------------------------------------------------------------
// | État courant     | Condition(s) de transition           | État suivant  |
// ---------------------------------------------------------------------------
// | FORWARD          | 0, >= 4 lignes détectées             | LOST          |
// |                  | isEventOnThree && 3 lignes détectées | LOST          |
// |                  | Sinon                                | FORWARD       |
// ---------------------------------------------------------------------------
// | ALIGN            | Ligne centrée détectée               |               |
// |                  |     isAutomatic                      | FORWARD       |
// |                  |     !isAutomatic                     | LOST          |
// |                  | Trop de tentatives échouées          | LOST          |
// ---------------------------------------------------------------------------
// | TURNING_LEFT     | Ligne détectée                       |               |
// |                  |     isAlignAfterTurn                 | ALIGN         |
// |                  |     !isAlignAfterTurn && isAutomatic | FORWARD       |
// |                  |     Sinon                            | LOST          |
// ---------------------------------------------------------------------------
// | TURNING_RIGHT    | Ligne détectée                       |               |
// |                  |     isAlignAfterTurn                 | ALIGN         |
// |                  |     !isAlignAfterTurn && isAutomatic | FORWARD       |
// |                  |     Sinon                            | LOST          |
// ---------------------------------------------------------------------------
// | LOST             | Aucun changement automatique         | LOST          |
// ---------------------------------------------------------------------------
// | STOP             | Arrêt demandé                        | STOP          |
// ---------------------------------------------------------------------------


#ifndef _LINE_FOLLOWER_H
#define _LINE_FOLLOWER_H

#include "LineSensor.h"
#include "MovementManager.h"

enum class LineFollowerState : uint8_t {
    FORWARD,
    ALIGN,
    TURNING_LEFT,
    TURNING_RIGHT,
    LOST,
    STOP
};

struct LineFollowerConfiguration {
    LineFollowerState state;
    bool isAutomatic : 1;
    bool isAlignAfterTurn : 1;
    bool isEventOnThree : 1;
    bool isSkippingStartingLine : 1;
    uint16_t adjustTimeMs;

    static const constexpr uint16_t TURN_WHEEL_ADJUST_TIME_SHORT_MS = 800;
    static const constexpr uint16_t TURN_WHEEL_ADJUST_TIME_LONG_MS = 1400;
};

template <typename T, typename U>
class LineFollower {
public:
    LineFollower(MovementManager<T, U>& movementManager, LineSensor& lineSensor,
                 float speed);
    ~LineFollower();
    LineFollower(LineFollower&) = delete;
    void operator=(const LineFollower&) = delete;

    void stop();
    void start(const LineFollowerConfiguration& configuration);
    void update(uint16_t deltaTimeMs);
    bool isLost() const;

private:
    static const constexpr uint16_t TURN_ALIGN_TIME_MS = 800;
    static const constexpr uint16_t ALIGN_TIME_MS = 25;
    static const constexpr uint8_t ALIGN_MAX_ATTEMPTS = 100;
    static const constexpr float TURN_SPEED_RATIO = 0.9F;
    static const constexpr float ALIGN_SPEED = 0.65F;
    static const constexpr float PID_KP = 0.175F;
    static const constexpr float PID_KI = 0.0F;
    static const constexpr float PID_KD = 7.5F;

    MovementManager<T, U>& movementManager_;
    LineSensor& lineSensor_;

    LineFollowerConfiguration configuration_;
    bool switchedState_;

    LineSensor::Readings lastReadings_;

    float speed_;

    // PID values
    int8_t lastError_;
    int16_t integralComponent_;

    // Turning values
    uint16_t adjustTimeLeft_, alignTimeLeft_;
    bool isExitingLine_, wasLostAndIsSkippingError_, hasFoundGuide_;
    uint8_t alignAttemptsLeft_;

    void forwardHandler(LineSensor::Readings readings);
    void alignHandler(LineSensor::Readings readings, uint16_t deltaTimeMs);
    void lostHandler();
    void turningHandler(LineSensor::Readings readings, uint16_t deltaTimeMs);
};

#endif /* _LINE_FOLLOWER_H */