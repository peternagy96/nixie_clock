/*
* Abstract class with basic time-keeping capabilities
*/

#ifndef __CHRONO_H
#define __CHRONO_H

#include <Arduino.h>

class ChronoClass {
   public:
    virtual void incrementHour(void) volatile = 0;
    virtual void decrementHour(void) volatile = 0;
    virtual void incrementMin(void) volatile = 0;
    virtual void decrementMin(void) volatile = 0;
    virtual void incrementSec(void) volatile = 0;
    virtual void decrementSec(void) volatile = 0;

    volatile int hour_t;
    volatile int minute_t;
    volatile int second_t;
    volatile int date_t;
    volatile int month_t;
    volatile int year_t;
};

#endif