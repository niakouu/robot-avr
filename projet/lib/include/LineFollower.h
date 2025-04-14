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

    // speed 0.5
    float PID_KP = 0.10F; // speed / 4 kind of
    float PID_KI = 0.0F;
    float PID_KD = 1.5F; // PID_KP * 15

private:
    static const constexpr uint16_t DETECTION_TIME_MS = 200;
    static const constexpr uint16_t ALIGN_TIME_MS = 200;
    static const constexpr uint8_t ALIGN_MAX_ATTEMPTS = 10;

    MovementManager<T, U>& movementManager_;
    LineSensor& lineSensor_;

    LineFollowerConfiguration configuration_;
    bool switchedState_;

    LineSensor::Readings lastReadings_;

public: // TODO change this
    float speed_;

    // PID values
    int8_t lastError_;
    int16_t integralComponent_;

    // Turning values
    uint16_t adjustTimeLeft_, alignTimeLeft_;
    bool isExitingLine_, wasLostAndIsSkippingError_, hasFoundGuide_;
    uint8_t alignAttemptsLeft_;

    void forwardHandler(LineSensor::Readings readings, uint16_t deltaTimeMs);
    void alignHandler(LineSensor::Readings readings, uint16_t deltaTimeMs);
    void lostHandler(LineSensor::Readings readings, uint16_t deltaTimeMs);
    void turningHandler(LineSensor::Readings readings, uint16_t deltaTimeMs);
};

#endif /* _LINE_FOLLOWER_H */