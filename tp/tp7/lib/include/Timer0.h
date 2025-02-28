#ifndef _TIMER0_H
#define _TIMER0_H

#include <stdint.h>

class Timer0 {
public:
    struct ConfigCounter
    {
        uint8_t max;
        uint8_t prescalar;
    };

    struct ConfigPwm
    {
        float ratios[2];
        uint8_t prescalar;
    };

    Timer0(const ConfigCounter& config);
    Timer0(const ConfigPwm& config);
    ~Timer0();

    void setConfig(const ConfigPwm& config);
    void setConfig(const ConfigCounter& config);

    void start();
    void stop();

private:
    
};

#endif /* _TIMER0_H */