/*
* Abstract class with basic time-keeping capabilities
*/

#ifndef __CHRONO_H
#define __CHRONO_H

class ChronoClass {
   public:
    void incrementDigit1(void) volatile = 0;
    bool decrementDigit1(void) volatile = 0;
    void incrementDigit2(void) volatile = 0;
    bool decrementDigit2(void) volatile = 0;
    void incrementDigit3(void) volatile = 0;
    bool decrementDigit3(void) volatile = 0;
    void incrementDigit4(void) volatile = 0;
    bool decrementDigit4(void) volatile = 0;

    void reset(void) volatile = 0;
    void copy(volatile ChronoClass *) volatile = 0;
    void roundup(void) volatile = 0;

    volatile int8_t second = 0;
    volatile int8_t minute = 0;
    volatile int8_t hour = 0;
};