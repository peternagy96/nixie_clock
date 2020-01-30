/*
* Abstract class with basic time-keeping capabilities
*/

#ifndef __CHRONO_H
#define __CHRONO_H

class ChronoClass {
   public:
    void incrementRight(void) volatile = 0;
    void decrementRight(void) volatile = 0;
    void incrementLeft(void) volatile = 0;
    void decrementLeft(void) volatile = 0;

    void reset(void) volatile = 0;
    void copy(volatile ChronoClass *) volatile = 0;
    void roundup(void) volatile = 0;
};

#endif