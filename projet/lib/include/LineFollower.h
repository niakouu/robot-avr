#ifndef _LINE_FOLLOWER_H
#define _LINE_FOLLOWER_H

#include "LineSensor.h"
#include "MovementManager.h"

template <typename T, typename U>
class LineFollower {
public:
    LineFollower(MovementManager<T, U>& movementManager, LineSensor& lineSensor,
                 float speed);
    ~LineFollower();
    LineFollower(LineFollower&) = delete;
    void operator=(const LineFollower&) = delete;

    void stop();
    void start();
    void update();
    bool isEvent() const;

private:
    enum class State : uint8_t {
        FORWARD,
        TURNING_LEFT,
        TURNING_RIGHT,
        LOST,
        STOP
    };

    MovementManager<T, U>& movementManager_;
    LineSensor& lineSensor_;
    State currentState_;
    float speed_;
    bool switchedState_;

    void setState(State state);
    void forwardHandler(LineSensor::Readings readings);
    void turningHandler(LineSensor::Readings readings);
};

#endif /* _LINE_FOLLOWER_H */