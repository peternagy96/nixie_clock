/*
 * Countdown timer implementation class
 */

#ifndef __COUNTDOWN_H
#define __COUNTDOWN_H

#include <Arduino.h>
#include "Nixie.h"

class CountdownClass {
   public:
    void initialize(int defaultMin, int defaultSec);
    void incrementSec(void) volatile;
    void decrementSec(void) volatile;
    void incrementMin(void) volatile;
    void decrementMin(void) volatile;
    void resetTime(void) volatile;
    void setDefault(void) volatile;
    void setTimeSlow(const char *var, const char *dir);
    void autoTurnoff(void) volatile;
    void displayTime(NixieDigits_s &timeDigits);
    void loopHandler(void);
    void stopwatch(void);

    void start(void);
    void restart(void);
    void stop(void);
    void resetAlarm(void);
    void reset(void);

    bool runningDown = false;
    bool runningUp = false;
    bool alarm = false;
    uint32_t setTs = 0;
    bool enabled = true;
    int8_t defaultSec;
    int8_t defaultMin;

   private:
    uint32_t alarmTs = 0;
    int8_t sec;
    int8_t min;
};

#endif
