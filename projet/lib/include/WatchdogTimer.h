//
// Travail : Projet Final
// Section # : 4
// Équipe # : 8290
// Correcteur : Dorine Dantrun
// ----------------------------------------------------------
// Développeurs : Edelina Alieva, Kyle Bouchard, Ilyes Belhaddad et Mohamed
// Elyes Bradai
//
// Description du debug.h :
//        Fournit une implémentation générique du timer watchdog, ainsi que des
//        utilitaires de repos. Favorise le watchdog timer pour des repos qui
//        rentre dans son intervalle, et _delay_ms lorsque c'est trop petit.

#ifndef _WATCHDOG_TIMER_H
#define _WATCHDOG_TIMER_H

#include <avr/wdt.h>
#include <stdint.h>

class WatchdogTimer {
public:
    enum class SleepMode : uint8_t {
        IDLE = SLEEP_MODE_IDLE,
        ADC_NOISE_REDUCTION = SLEEP_MODE_ADC,
        POWER_DOWN = SLEEP_MODE_PWR_DOWN,
        POWER_SAVE = SLEEP_MODE_PWR_SAVE,
        STANDBY = SLEEP_MODE_STANDBY,
        EXTENDED_STANDBY = SLEEP_MODE_EXT_STANDBY
    };

    WatchdogTimer(WatchdogTimer&) = delete;
    void operator=(const WatchdogTimer&) = delete;

    /// @brief Sleep for a specific duration and a power saving mode.
    /// @param milliseconds Duration of sleep in milliseconds.
    /// @param sleepMode Power saving mode to sleep in.
    /// constants.
    void sleep(const uint16_t milliseconds, const SleepMode sleepMode);
    void setSleepDone();

protected:
    friend class Board;
    WatchdogTimer();
    ~WatchdogTimer();

private:
    volatile bool sleepDone_;

    /// @brief Sleep for a predefined duration and a power saving mode.
    /// @param durationMode Select from `avr/wdt.h`: One of the `WDTO_*`
    /// constants.
    /// @param sleepMode Select from `avr/io.h`: One of the `SLEEP_MODE_*`
    /// constants.
    void rawSleep(const uint8_t durationMode, const SleepMode sleepMode);

    static void wdtEnableInterrupt(const uint8_t durationMode);
};

#endif /* _WATCHDOG_TIMER_H */