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

    /*
    The increment methods implement proper timekeeping,
    so only incrementing seconds keeps track of time.
    Only decrementing seconds has an effect minutes,
    other decrement methods are standalone.
    */
    void incrementSec(void) volatile;
    void decrementSec(void) volatile;
    void incrementMin(void) volatile;
    void decrementMin(void) volatile;
    void incrementHour(void) volatile;
    void decrementHour(void) volatile;
    void incrementDay(void) volatile;
    void decrementDay(void) volatile;
    void incrementMonth(void) volatile;
    void decrementMonth(void) volatile;
    void incrementYear(void) volatile;
    void decrementYear(void) volatile;
    bool isShortMonth(void) volatile;
    bool isLeapYear(void) volatile;

    void reset(void) volatile;
    void copy(volatile ChronoClass *) volatile;
    void update(void);

    Time *time;
    volatile int8_t second = 0;
    volatile int8_t minute = 0;
    volatile int8_t hour = 0;
    volatile int8_t day = 0;
    volatile int8_t month = 0;
    volatile int16_t year = 0;
    volatile int8_t short_months[4] = {4, 6, 9, 11};
};

extern TimekeeperClass Timekeeper;

#endif