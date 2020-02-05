/*
 * Alarm clock implementation class
 */

#ifndef __ALARM_H
#define __ALARM_H

#include "Chrono.h"
#include "Timekeeper.h"

#include <stdint.h>

class AlarmClass : public ChronoClass {
   public:
    void initialize(void);
    void resetAlarm(void);
    void incrementSec(void) volatile;
    void decrementSec(void) volatile;
    void incrementMin(void) volatile;
    void decrementMin(void) volatile;
    void incrementHour(void) volatile;
    void decrementHour(void) volatile;
    void alarmGoesOff(void) volatile;

    bool alarm = false;
    bool active = false;

    int8_t hour = 0;
    int8_t minute = 0;

   private:
    volatile TimekeeperClass tm;
};

#endif