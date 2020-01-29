
#include "Timekeeper.h"

void TimekeeperClass::incrementRight(void) volatile {
    minute++;
    if (minute > 59) minute = 0, hour++;
}

void TimekeeperClass::decrementRight(void) volatile {
    minute--;
    if (minute < 0) minute = 59, hour--;
    if (hour < 0) hour = 23;
}

void TimekeeperClass::incrementLeft(void) volatile {
    hour++;
    if (hour > 23) hour = 0;
}

void TimekeeperClass::decrementLeft(void) volatile {
    hour--;
    if (hour < 0) hour = 23, hour--;
}

void TimekeeperClass::reset(void) volatile {
    minute = 0;
    hour = 0;
}

void TimekeeperClass::copy(volatile ChronoClass *tm) volatile {
    minute = tm->minute;
    hour = tm->hour;
}

void TimekeeperClass::roundup(void) volatile {
    if (minute > 59) minute = 0, hour++;
    if (hour > 23) hour = 0;
}