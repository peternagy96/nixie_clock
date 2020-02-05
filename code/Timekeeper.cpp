
#include "Timekeeper.h"
#include "Chrono.h"
#include "DS1302.h"

TimekeeperClass Timekeeper;

void TimekeeperClass::initialize(Time *time) {
    this->time = time;
}

void TimekeeperClass::update(void) {
    time->sec = second;
    time->min = minute;
    time->hr = hour;
    time->day = day;
    time->mon = month;
    time->yr = year;
}

void TimekeeperClass::displayTime(NixieDigits_s &timeDigits) {
    update();
    timeDigits.value[0] = (time->min / 1U) % 10;
    timeDigits.value[1] = (time->min / 10U) % 10;
    timeDigits.value[3] = (time->hr / 1U) % 10;
    timeDigits.value[4] = (time->hr / 10U) % 10;
}

void TimekeeperClass::displayDate(NixieDigits_s &timeDigits) {
    update();
    timeDigits.value[0] = (time->date / 1U) % 10;
    timeDigits.value[1] = (time->date / 10U) % 10;
    timeDigits.value[3] = (time->mon / 1U) % 10;
    timeDigits.value[4] = (time->mon / 10U) % 10;
}

void TimekeeperClass::displayYear(NixieDigits_s &timeDigits) {
    update();
    timeDigits.value[0] = (time->yr / 1U) % 10;
    timeDigits.value[1] = (time->yr / 10U) % 10;
    timeDigits.value[3] = (time->yr / 100U) % 10;
    timeDigits.value[4] = (time->yr / 1000U) % 10;
}

void TimekeeperClass::incrementSec(void) volatile {
    second++;
    if (second > 59) second = 0, incrementMin();
}

void TimekeeperClass::decrementSec(void) volatile {
    second++;
    if (second < 0) second = 59, decrementMin();
}

void TimekeeperClass::incrementMin(void) volatile {
    minute++;
    if (minute > 59) minute = 0, incrementHour();
}

void TimekeeperClass::decrementMin(void) volatile {
    minute--;
    if (minute < 0) minute = 59;
}

void TimekeeperClass::incrementHour(void) volatile {
    hour++;
    if (hour > 23) hour = 0, incrementDay();
}

void TimekeeperClass::decrementHour(void) volatile {
    hour--;
    if (hour < 0) hour = 23;
}

void TimekeeperClass::incrementDay(void) volatile {
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

void TimekeeperClass::decrementDay(void) volatile {
    day--;
    if (day < 0) {
        if (month == 2 && !isLeapYear()) {
            day = 28;
        } else if (month == 2 && isLeapYear()) {
            day = 29;
        } else if (day > 30 && isShortMonth()) {
            day = 30;
        } else if (day > 31) {
            day = 31;
        }
    }
}

bool TimekeeperClass::isLeapYear(void) volatile {
    if ((year - 2020) % 4 == 0) {
        return true;
    } else {
        return false;
    }
}

bool TimekeeperClass::isShortMonth(void) volatile {
    for (int8_t i = 0; i < 4; i++) {
        if (month == short_months[i]) {
            return true;
        }
    }
    return false;
}

void TimekeeperClass::reset(void) volatile {
    second = 0;
    minute = 0;
    hour = 0;
    day = 1;
    month = 1;
    year = 2020;
}

void TimekeeperClass::copy(volatile ChronoClass *tm) volatile {
    second = tm->second;
    minute = tm->minute;
    hour = tm->hour;
    day = tm->day;
    month = tm->month;
    year = tm->year;
}