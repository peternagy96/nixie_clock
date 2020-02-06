
#include "Timekeeper.h"
#include <Arduino.h>
#include <Time.h>
#include "Chrono.h"
#include "DS1302.h"

#define INCREMENT_TIME (500)

TimekeeperClass Timekeeper;

void TimekeeperClass::initialize(Time *time) {
    this->time = time;
    second = time->sec;
    minute = time->min;
    hour = time->hr;
    date = time->date;
    month = time->mon;
    year = time->yr;
}

void TimekeeperClass::update(void) {
    time->sec = second;
    time->min = minute;
    time->hr = hour;
    time->date = date;
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
    if (!isBeingSet) {
        second++;
        if (second > 59) second = 0, incrementMin();
    }
}

void TimekeeperClass::decrementSec(void) volatile {
    second--;
    if (second < 0) {
        second = 59;
        if (!isBeingSet) {
            decrementMin();
        }
    }
}

void TimekeeperClass::setSecToNull(void) volatile {
    second = 0;
    wasSet = false;
}

void TimekeeperClass::incrementMin(void) volatile {
    minute++;
    if (minute > 59) {
        minute = 0;
        if (!isBeingSet) {
            incrementHour();
        }
    }
}

void TimekeeperClass::decrementMin(void) volatile {
    minute--;
    if (minute < 0) {
        minute = 59;
        if (!isBeingSet) {
            decrementHour();
        }
    }
}

void TimekeeperClass::incrementHour(void) volatile {
    hour++;
    if (hour > 23) {
        hour = 0;
        if (!isBeingSet) {
            incrementDate();
        }
    }
}

void TimekeeperClass::decrementHour(void) volatile {
    hour--;
    if (hour < 0) {
        hour = 23;
        if (!isBeingSet) {
            decrementDate();
        }
    }
}

void TimekeeperClass::incrementDate(void) volatile {
    date++;
    if (month == 2 && !isLeapYear() && date > 28) {
        if (!isBeingSet) {
            month++;
        }
        date = 1;
    } else if (month == 2 && isLeapYear() && date > 29) {
        if (!isBeingSet) {
            month++;
        }
        date = 1;
    } else if (date > 30 && isShortMonth()) {
        if (!isBeingSet) {
            month++;
        }
        date = 1;
    } else if (date > 31) {
        if (!isBeingSet) {
            month++;
        }
        date = 1;
    }
    if (month > 12 && !isBeingSet) month = 1, year++;
}

void TimekeeperClass::decrementDate(void) volatile {
    date--;
    if (date < 0) {
        if (month == 2 && !isLeapYear()) {
            date = 28;
        } else if (month == 2 && isLeapYear()) {
            date = 29;
        } else if (date > 30 && isShortMonth()) {
            date = 30;
        } else if (date > 31) {
            date = 31;
        }
    }
}

void TimekeeperClass::incrementMonth(void) volatile {
    month++;
    if (month > 12) month = 1;
}

void TimekeeperClass::decrementMonth(void) volatile {
    month--;
    if (month < 0) month = 12;
}

void TimekeeperClass::incrementYear(void) volatile {
    year++;
    if (year > 9999) year = 2020;
}

void TimekeeperClass::decrementYear(void) volatile {
    year--;
    if (year < 0) year = 9999;
}

void TimekeeperClass::setTimeSlow(const char *var, const char *dir) volatile {
    if (millis() - setTs > INCREMENT_TIME) {
        if (var == "sec") {
            if (dir == "+") {
                incrementSec();
            } else if (dir == "-") {
                decrementSec();
            }
        } else if (var == "min") {
            if (dir == "+") {
                incrementMin();
            } else if (dir == "-") {
                decrementMin();
            }
        } else if (var == "hour") {
            if (dir == "+") {
                incrementHour();
            } else if (dir == "-") {
                decrementHour();
            }
        } else if (var == "date") {
            if (dir == "+") {
                incrementDate();
            } else if (dir == "-") {
                decrementDate();
            }
        } else if (var == "month") {
            if (dir == "+") {
                incrementMonth();
            } else if (dir == "-") {
                decrementMonth();
            }
        } else if (var == "year") {
            if (dir == "+") {
                incrementYear();
            } else if (dir == "-") {
                decrementYear();
            }
        }
        setTs = millis();
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
    date = 1;
    month = 1;
    year = 2020;
}

void TimekeeperClass::copy(volatile ChronoClass *tm) volatile {
    second = tm->second;
    minute = tm->minute;
    hour = tm->hour;
    date = tm->date;
    month = tm->month;
    year = tm->year;
}