/*
* Class to keep track of the date
*/

#ifndef __DATEKEEPER_H
#define __DATEKEEPER_H

class DatekeeperClass : public ChronoClass {
   public:
    void incrementRight(void) volatile;
    void decrementRight(void) volatile;
    void incrementLeft(void) volatile;
    void decrementLeft(void) volatile;

    void reset(void) volatile;
    void copy(volatile ChronoClass *) volatile;
    void roundup(void) volatile;
    bool isShortMonth(void) volatile;

    volatile int8_t day = 0;
    volatile int8_t month = 0;
    volatile int8_t year = 0;
    volatile int8_t short_months[] = {4, 6, 9, 11};
};