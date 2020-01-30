
#include "Datekeeper.h"
#include "Chrono.h"

void DatekeeperClass::incrementRight(void) volatile {
    day++;
    if (month == 2 && !isLeapYear() && day > 28) {
        month++, day = 1;
    } else if (month == 2 && isLeapYear() && day > 29) {
        month++, day = 1;
    } else if (day > 30 && isShortMonth()) {
        month++, day = 1;
    } else if (day > 31) {
        month++, day = 1;
    }
    if (month > 12) month = 1, year++;
}

void DatekeeperClass::decrementRight(void) volatile {
    minute--;
    if (minute < 0) minute = 59, hour--;
    if (hour < 0) hour = 23;
}

void DatekeeperClass::incrementLeft(void) volatile {
    hour++;
    if (hour > 23) hour = 0;
}

void DatekeeperClass::decrementLeft(void) volatile {
    hour--;
    if (hour < 0) hour = 23, hour--;
}

bool DatekeeperClass::isLeapYear(void) volatile {
    if ((year - 2020) % 4 == 0) {
        return true;
    } else {
        return false;
    }
}

bool DatekeeperClass::isShortMonth(void) volatile {
    for (int8_t i = 0; i < 4; i++) {
        if (month == short_months[i]) {
            return true;
        }
    }
    return false;
}

void DatekeeperClass::reset(void) volatile {
    day = 1;
    month = 1;
    year = 2020;
}

void DatekeeperClass::copy(volatile ChronoClass *tm) volatile {
    day = tm->minute;
    month = tm->hour;
}

void DatekeeperClass::roundup(void) volatile {
}