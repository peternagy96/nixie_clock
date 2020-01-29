
#include "Datekeeper.h"
#include <algorithm>  // for std::find
#include <iterator>   // for std::begin, std::end

void DatekeeperClass::incrementRight(void) volatile {
    day++;
    bool exists = std::find(std::begin(a), std::end(a), x) != std::end(a);
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

void isLeapYear(void) volatile {
    if ((year - 2020) % 4 == 0) {
        return true;
    } else {
        return false;
    }
}

void isShortMonth(void) volatile {
    return std::find(std::begin(short_months), std::end(short_months), month) != std::end(short_months)
}

void DatekeeperClass::reset(void) volatile {
    minute = 0;
    hour = 0;
}

void DatekeeperClass::copy(volatile ChronoClass *tm) volatile {
    minute = tm->minute;
    hour = tm->hour;
}

void DatekeeperClass::roundup(void) volatile {
    if (minute > 59) minute = 0, hour++;
    if (hour > 23) hour = 0;
}