/*
* Abstract class with basic time-keeping capabilities
*/

#ifndef __CHRONO_H
#define __CHRONO_H

class ChronoClass {
   public:
    virtual void incrementRight(void) volatile = 0;
    virtual void decrementRight(void) volatile = 0;
    virtual void incrementLeft(void) volatile = 0;
    virtual void decrementLeft(void) volatile = 0;

    volatile int8_t minute;
    volatile int8_t hour;
};

#endif