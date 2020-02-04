
#include "Timekeeper.h"
#include "Chrono.h"
#include "DS1302.h"

TimekeeperClass Timekeeper;

void TimekeeperClass::initialize(Time *time) {
    this->time = time;
}

void TimekeeperClass::displayTime(NixieDigits_s &timeDigits) {
    timeDigits.value[0] = (time->min / 1U) % 10;
    timeDigits.value[1] = (time->min / 10U) % 10;
    timeDigits.value[3] = (time->hr / 1U) % 10;
    timeDigits.value[4] = (time->hr / 10U) % 10;
}

void TimekeeperClass::displayDate(NixieDigits_s &timeDigits) {
    timeDigits.value[0] = (time->date / 1U) % 10;
    timeDigits.value[1] = (time->date / 10U) % 10;
    timeDigits.value[3] = (time->mon / 1U) % 10;
    timeDigits.value[4] = (time->mon / 10U) % 10;
}

void TimekeeperClass::displayYear(NixieDigits_s &timeDigits) {
    timeDigits.value[0] = (time->yr / 1U) % 10;
    timeDigits.value[1] = (time->yr / 10U) % 10;
    timeDigits.value[3] = (time->yr / 100U) % 10;
    timeDigits.value[4] = (time->yr / 1000U) % 10;
}

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