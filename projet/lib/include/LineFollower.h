#ifndef _LINE_FOLLOWER_H
#define _LINE_FOLLOWER_H

#include "LineSensor.h"
#include "MovementManager.h"


enum class LineFollowerState : uint8_t {
    FORWARD,
    DETECTION,
    TURNING_LEFT,
    TURNING_RIGHT,
    LOST,
    STOP
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
    void start(LineFollowerState state = LineFollowerState::FORWARD);
    void update(uint16_t deltaTimeMs);
    bool isEvent() const;

    // speed 0.5
    float PID_KP = 0.135F; // speed / 4 kind of
    float PID_KI = 0.0675F; // PID_KP / 20
    float PID_KD = 2.025F; // PID_KP * 15

private:
    static const constexpr uint16_t DETECTION_TIME_MS = 200;

    MovementManager<T, U>& movementManager_;
    LineSensor& lineSensor_;

    LineFollowerState currentState_;
    bool switchedState_;

    LineSensor::Readings lastReadings_;

    public: // TODO change this
    float speed_;

    // PID values
    int8_t lastError_;
    int16_t integralComponent_;

    // Turning values
    uint16_t timeLeft_;

    void forwardHandler(LineSensor::Readings readings, uint16_t deltaTimeMs);
    void detectionHandler(LineSensor::Readings readings, uint16_t deltaTimeMs);
    void lostHandler(LineSensor::Readings readings, uint16_t deltaTimeMs);
    void turningHandler(LineSensor::Readings readings, uint16_t deltaTimeMs);
};

#endif /* _LINE_FOLLOWER_H */