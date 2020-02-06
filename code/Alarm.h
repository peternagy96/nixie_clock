/*
 Alarm clock implementation class
 */

#ifndef __ALARM_H
#define __ALARM_H

#include "Timekeeper.h"

#include <stdint.h>

class AlarmClass {
   public:
    void initialize(void);
    void resetAlarm(void);
    void alarmGoesOff(void);
    void setTimeSlow(const char *var, const char *dir);
    void autoTurnoff(void) volatile;
    void displayTime(NixieDigits_s &timeDigits);

    bool alarm = false;

    uint8_t hour = 0;
    uint8_t minute = 0;
    uint32_t setTs = 0;

   private:
    uint32_t alarmTs = 0;
    volatile TimekeeperClass tm;
};

#endif