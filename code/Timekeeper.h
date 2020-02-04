/*
* Class to keep track of time
*/

#ifndef __TIMEKEEPER_H
#define __TIMEKEEPER_H

#include <stdint.h>
#include "Chrono.h"
#include "DS1302.h"
#include "Nixie.h"

class TimekeeperClass : public ChronoClass {
   public:
    void initialize(Time *time);
    void displayTime(NixieDigits_s &timeDigits);
    void displayDate(NixieDigits_s &timeDigits);
    void displayYear(NixieDigits_s &timeDigits);
    void incrementRight(void) volatile;
    void decrementRight(void) volatile;
    void incrementLeft(void) volatile;
    void decrementLeft(void) volatile;

    void reset(void) volatile;
    void copy(volatile ChronoClass *) volatile;
    void roundup(void) volatile;

    Time *time;
    volatile int8_t second = 0;
    volatile int8_t minute = 0;
    volatile int8_t hour = 0;
};

extern TimekeeperClass Timekeeper;

#endif