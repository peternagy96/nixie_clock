/*
 * Countdown timer implementation class
 */

#ifndef __COUNTDOWN_H
#define __COUNTDOWN_H

#include <stdint.h>
#include "Nixie.h"

class CountdownClass {
   public:
    void initialize();
    void incrementSec(void) volatile;
    void decrementSec(void) volatile;
    void incrementMin(void) volatile;
    void decrementMin(void) volatile;
    void setTimeSlow(const char *var, const char *dir);
    void autoTurnoff(void) volatile;
    void displayTime(NixieDigits_s &timeDigits);
    void loopHandler(void);
    void stopwatch(void);

    void start(void);
    void stop(void);
    void resetAlarm(void);
    void reset(void);

    bool runningDown = false;
    bool runningUp = false;
    bool alarm = false;
    uint32_t setTs = 0;

   private:
    uint32_t alarmTs = 0;
    int defaultSec;
    int defaultMin;
    int sec;
    int min;
};

#endif
