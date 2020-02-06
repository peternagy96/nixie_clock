/*
 * Countdown timer implementation class
 */

#ifndef __TIMER_H
#define __TIMER_H

#include <stdint.h>
#include "Timekeeper.h"

class TimerClass {
   public:
    void initialize();
    void incrementSec(void) volatile;
    void decrementSec(void) volatile;
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
    TimekeeperClass defaultTm;
    volatile TimekeeperClass tm;
};

#endif
