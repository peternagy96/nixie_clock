/*
* Alarm class to implement features for 2 independent alarms
*/

#include "Alarm.h"

void AlarmClass::initialize(AlarmEeprom_s *settings) {
    alarm = false;
    tm.hour = 8;
    tm.minute = 0;
}

void AlarmClass::alarmGoesOff(int8_t hour, int8_t minute) {
    if (hour == tm.hour && minute == tm.minute && !active) {
        active = true;
    }
}

void AlarmClass::resetAlarm(void) {
    alarm = false;
}

void AlarmClass::incrementRight(void) {
    tm.incrementRight();
}

void AlarmClass::decrementRight(void) {
    tm.decrementRight();
}

void AlarmClass::incrementLeft(void) {
    tm.incrementLeft();
}

void AlarmClass::decrementLeft(void) {
    tm.decrementLeft();
}
