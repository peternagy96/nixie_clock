/*
 Alarm class to implement features for 2 independent alarms
*/

#include "Alarm.h"

#define ALARM_TIMEOUT 120000
#define INCREMENT_TIME 500

void AlarmClass::initialize(void) {
    alarm = false;
    tm.hour = 8;
    tm.minute = 0;
}

void AlarmClass::displayTime(NixieDigits_s &timeDigits) {
    timeDigits.value[0] = (tm.minute / 1U) % 10;
    timeDigits.value[1] = (tm.minute / 10U) % 10;
    timeDigits.value[3] = (tm.hour / 1U) % 10;
    timeDigits.value[4] = (tm.hour / 10U) % 10;
}

void AlarmClass::alarmGoesOff(Time time) {
    if (time.hr == tm.hour && time.min == tm.minute && !alarm && active) {
        active = false;
        alarm = true;
        alarmTs = millis();
    }
}

void AlarmClass::resetAlarm(void) {
    alarm = false;
}

void AlarmClass::autoTurnoff(void) volatile {
    if ((millis() - alarmTs >= ALARM_TIMEOUT) && alarm) {
        alarm = false;
    }
}

void AlarmClass::setTimeSlow(const char *var, const char *dir) {
    if (millis() - setTs >= INCREMENT_TIME) {
        if (var == "min") {
            if (dir == "+") {
                tm.incrementMin();
            } else if (dir == "-") {
                tm.decrementMin();
            }
        } else if (var == "hour") {
            if (dir == "+") {
                tm.incrementHour();
            } else if (dir == "-") {
                tm.decrementHour();
            }
        }
        setTs = millis();
    }
}
