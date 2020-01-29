/*
* Abstract class with basic time-keeping capabilities
*/

#ifndef __CHRONO_H
#define __CHRONO_H

class ChronoClass {
   public:
    void setRight(void) volatile = 0;
    void setLeft(void) volatile = 0;

    void reset(void) volatile = 0;
    void copy(volatile ChronoClass *) volatile = 0;
    void roundup(void) volatile = 0;
};