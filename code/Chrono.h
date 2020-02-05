/*
* Abstract class with basic time-keeping capabilities
*/

#ifndef __CHRONO_H
#define __CHRONO_H

#include <stdint.h>

class ChronoClass {
   public:
    virtual void incrementHour(void) volatile = 0;
    virtual void decrementHour(void) volatile = 0;
    virtual void incrementMin(void) volatile = 0;
    virtual void decrementMin(void) volatile = 0;
    virtual void incrementSec(void) volatile = 0;
    virtual void decrementSec(void) volatile = 0;

    volatile int8_t hour;
    volatile int8_t minute;
    volatile int8_t second;
    volatile int8_t day;
    volatile int8_t month;
    volatile int8_t year;
};

#endif