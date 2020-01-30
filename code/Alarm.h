/*
 * Alarm clock implementation class
 */

#ifndef __ALARM_H
#define __ALARM_H

#include "Chrono.h"
#include "Timekeeper.h"

class AlarmClass : public ChronoClass {
   public:
    void initialize(void);
    void resetAlarm(void);
    void incrementRight(void) volatile;
    void decrementRight(void) volatile;
    void incrementLeft(void) volatile;
    void decrementLeft(void) volatile;
    void alarmGoesOff(void) volatile;

    bool alarm = false;
    bool active = false;

   private:
    volatile TimekeeperClass tm;
};

#endif