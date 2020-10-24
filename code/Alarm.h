/*
 Alarm clock implementation class
 */

#ifndef __ALARM_H
#define __ALARM_H

#include "Timekeeper.h"
#include <TimeLib.h>

class AlarmClass {
   public:
    void initialize(void);
    void resetAlarm(void);
    void alarmGoesOff();
    void setTimeSlow(const char *var, const char *dir);
    void autoTurnoff(void) volatile;
    void displayTime(NixieDigits_s &timeDigits);

    bool alarm = false;
    bool active = false;
    bool isBeingSet = false;

    int hour_t = 0;
    int minute_t = 0;
    uint32_t setTs = 0;

   private:
    uint32_t alarmTs = 0;
    volatile TimekeeperClass tm;
};

#endif