/*
* Alarm class to implement features for 2 independent alarms
*/

#include "Alarm.h"
#include <stdint.h>

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

void AlarmClass::incrementRight(void) volatile {
    tm.incrementRight();
}

void AlarmClass::decrementRight(void) volatile {
    tm.decrementRight();
}

void AlarmClass::incrementLeft(void) volatile {
    tm.incrementLeft();
}

void AlarmClass::decrementLeft(void) volatile {
    tm.decrementLeft();
}
