/*
* Class to keep track of time
*/

#ifndef __TIMEKEEPER_H
#define __TIMEKEEPER_H

#include <Arduino.h>
#include <TimeLib.h>
#include <DS3232RTC.h> 

#include "Chrono.h"
#include "Nixie.h"

class TimekeeperClass : public ChronoClass {
   public:
    void initialize(void);
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
    void setSecToNull(void) volatile;
    void incrementMin(void) volatile;
    void decrementMin(void) volatile;
    void incrementHour(void) volatile;
    void decrementHour(void) volatile;
    void incrementDate(void) volatile;
    void decrementDate(void) volatile;
    void incrementMonth(void) volatile;
    void decrementMonth(void) volatile;
    void incrementYear(void) volatile;
    void decrementYear(void) volatile;
    bool isShortMonth(void) volatile;
    bool isLeapYear(void) volatile;
    void roundDate(void) volatile;

    void setTimeSlow(const char *var, const char *dir) volatile;

    void reset(void) volatile;
    void copy(volatile ChronoClass *) volatile;
    void update(void);

    volatile int second_t = 0;
    volatile int minute_t = 0;
    volatile int hour_t = 0;
    volatile int date_t = 0;
    volatile int month_t = 0;
    volatile int year_t = 0;
    volatile int short_months[4] = {4, 6, 9, 11};
    volatile uint32_t setTs = 0;
    bool isBeingSet = false;
    bool wasSet = false;
};

extern TimekeeperClass Timekeeper;

#endif