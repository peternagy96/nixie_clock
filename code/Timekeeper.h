/*
* Class to keep track of time
*/

#ifndef __TIMEKEEPER_H
#define __TIMEKEEPER_H

class TimekeeperClass : public ChronoClass {
   public:
    void incrementRight(void) volatile;
    void decrementRight(void) volatile;
    void incrementLeft(void) volatile;
    void decrementLeft(void) volatile;

    void reset(void) volatile;
    void copy(volatile ChronoClass *) volatile;
    void roundup(void) volatile;

    volatile int8_t minute = 0;
    volatile int8_t hour = 0;
};

#endif