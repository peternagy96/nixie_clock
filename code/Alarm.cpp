/*
* Alarm class to implement features for 2 independent alarms
*/

#include "Alarm.h"
#include <stdint.h>
#include "Timekeeper.h"

void AlarmClass::initialize(void) {
    alarm = false;
    tm.hour = 8;
    tm.minute = 0;
}

void AlarmClass::alarmGoesOff(void) volatile {
    if (hour == tm.hour && minute == tm.minute && !active) {
        active = true;
    }
}

void AlarmClass::resetAlarm(void) {
    alarm = false;
}

void AlarmClass::incrementSec(void) volatile {
    tm.incrementSec();
}

void AlarmClass::decrementSec(void) volatile {
    tm.decrementSec();
}

void AlarmClass::incrementMin(void) volatile {
    tm.incrementMin();
}

void AlarmClass::decrementMin(void) volatile {
    tm.decrementMin();
}

void AlarmClass::incrementHour(void) volatile {
    tm.incrementHour();
}

void AlarmClass::decrementHour(void) volatile {
    tm.decrementHour();
}
