/*
 * Countdown timer implementation class
 */

#ifndef __TIMER_H
#define __TIMER_H

#include <stdint.h>
#include "Chrono.h"
#include "Timekeeper.h"

class TimerClass : public ChronoClass {
   public:
    void initialize();
    void incrementRight(void) volatile;
    void decrementRight(void) volatile;
    void incrementLeft(void) volatile;
    void decrementLeft(void) volatile;
    bool alarmGoesOff(void) volatile;

    void start(void);
    void stop(void);
    void resetAlarm(void);
    void reset(void);

    volatile bool running = false;
    volatile bool alarm = false;

   private:
    uint32_t alarmTs = 0;  // ToDo: implement timeout for the alarm
    TimekeeperClass defaultTm;
    volatile TimekeeperClass tm;
};

#endif
