
#include "Timekeeper.h"

#define INCREMENT_TIME 500

TimekeeperClass Timekeeper;

void TimekeeperClass::initialize(void) {
    second_t = second();
    minute_t = minute();
    hour_t = hour();
    date_t = day();
    month_t = month();
    year_t = year();
}

void TimekeeperClass::update(void) {
    year_t = year();
    month_t = month();
    date_t = day();
    hour_t = hour();
    minute_t = minute();
    second_t = second();
}

void TimekeeperClass::displayTime(NixieDigits_s &timeDigits) {
    update();
    timeDigits.value[0] = (minute_t / 1U) % 10;
    timeDigits.value[1] = (minute_t / 10U) % 10;
    timeDigits.value[3] = (hour_t / 1U) % 10;
    timeDigits.value[4] = (hour_t / 10U) % 10;
}

void TimekeeperClass::displayDate(NixieDigits_s &timeDigits) {
    update();
    timeDigits.value[0] = (date_t / 1U) % 10;
    timeDigits.value[1] = (date_t / 10U) % 10;
    timeDigits.value[3] = (month_t / 1U) % 10;
    timeDigits.value[4] = (month_t / 10U) % 10;
}

void TimekeeperClass::displayYear(NixieDigits_s &timeDigits) {
    update();
    timeDigits.value[0] = (year_t / 1U) % 10;
    timeDigits.value[1] = (year_t / 10U) % 10;
    timeDigits.value[3] = (year_t / 100U) % 10;
    timeDigits.value[4] = (year_t / 1000U) % 10;
}

void TimekeeperClass::incrementSec(void) volatile {
    if (!isBeingSet) {
        second_t++;
        if (second_t > 59) second_t = 0, incrementMin();
    }
}

void TimekeeperClass::decrementSec(void) volatile {
    second_t--;
    if (second_t < 0) {
        second_t = 59;
        if (!isBeingSet) {
            decrementMin();
        }
    }
}

void TimekeeperClass::setSecToNull(void) volatile {
    second_t = 0;
    wasSet = false;
}

void TimekeeperClass::incrementMin(void) volatile {
    minute_t++;
    if (minute_t > 59) {
        minute_t = 0;
        if (!isBeingSet) {
            incrementHour();
        }
    }
}

void TimekeeperClass::decrementMin(void) volatile {
    minute_t--;
    if (minute_t < 0) {
        minute_t = 59;
        if (!isBeingSet) {
            decrementHour();
        }
    }
}

void TimekeeperClass::incrementHour(void) volatile {
    hour_t++;
    if (hour_t > 23) {
        hour_t = 0;
        if (!isBeingSet) {
            incrementDate();
        }
    }
}

void TimekeeperClass::decrementHour(void) volatile {
    hour_t--;
    if (hour_t < 0) {
        hour_t = 23;
        if (!isBeingSet) {
            decrementDate();
        }
    }
}

void TimekeeperClass::incrementDate(void) volatile {
    date_t++;
    if (month_t == 2 && !isLeapYear() && date_t > 28) {
        if (!isBeingSet) {
            month_t++;
        }
        date_t = 1;
    } else if (month_t == 2 && isLeapYear() && date_t > 29) {
        if (!isBeingSet) {
            month_t++;
        }
        date_t = 1;
    } else if (date_t > 30 && isShortMonth()) {
        if (!isBeingSet) {
            month_t++;
        }
        date_t = 1;
    } else if (date_t > 31) {
        if (!isBeingSet) {
            month_t++;
        }
        date_t = 1;
    }
    if (month_t > 12 && !isBeingSet) month_t = 1, year_t++;
}

void TimekeeperClass::decrementDate(void) volatile {
    date_t--;
    if (date_t < 1) {
        if (month_t == 2 && !isLeapYear()) {
            date_t = 28;
        } else if (month_t == 2 && isLeapYear()) {
            date_t = 29;
        } else if (isShortMonth()) {
            date_t = 30;
        } else {
            date_t = 31;
        }
    }
}

void TimekeeperClass::incrementMonth(void) volatile {
    month_t++;
    if (month_t > 12) {
        month_t = 1;
    }
    roundDate();
}

void TimekeeperClass::decrementMonth(void) volatile {
    month_t--;
    if (month_t < 1) {
        month_t = 12;
    }
    //roundDate();
}

void TimekeeperClass::roundDate(void) volatile {
    if (date_t > 28 && month_t == 2 && !isLeapYear()) {
        date_t = 28;
    } else if (date_t > 29 && month_t == 2 && isLeapYear()) {
        date_t = 29;
    } else if (date_t > 30 && isShortMonth()) {
        date_t = 30;
    } else if (date_t > 31) {
        date_t = 31;
    }
}

void TimekeeperClass::incrementYear(void) volatile {
    year_t++;
    if (year_t > 9999) year_t = 2020;
}

void TimekeeperClass::decrementYear(void) volatile {
    year_t--;
    if (year_t < 0) year_t = 9999;
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
    tmElements_t tm; 
    tm.Year = CalendarYrToTm(year_t);
    tm.Month = month_t;
    tm.Day = date_t;
    tm.Hour = hour_t;
    tm.Minute = minute_t;
    tm.Second = second_t;

    time_t newTm;
    newTm = makeTime(tm);
    RTC.set(newTm);
    setTime(newTm);
}

bool TimekeeperClass::isLeapYear(void) volatile {
    if ((year_t - 2020) % 4 == 0) {
        return true;
    } else {
        return false;
    }
}

bool TimekeeperClass::isShortMonth(void) volatile {
    for (int i = 0; i < 4; i++) {
        if (month_t == short_months[i]) {
            return true;
        }
    }
    return false;
}

void TimekeeperClass::reset(void) volatile {
    second_t = 0;
    minute_t = 0;
    hour_t = 0;
    date_t = 1;
    month_t = 1;
    year_t = 2020;
}

void TimekeeperClass::copy(volatile ChronoClass *tm) volatile {
    second_t = tm->second_t;
    minute_t = tm->minute_t;
    hour_t = tm->hour_t;
    date_t = tm->date_t;
    month_t = tm->month_t;
    year_t = tm->year_t;
}