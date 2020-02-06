/*
* Timer class to implement countdown timer
*/

#include "Timer.h"
#include "Arduino.h"
#include "Nixie.h"

#define ALARM_TIMEOUT (120000)
#define INCREMENT_TIME (500)

void TimerClass::initialize() {
    defaultTm.second = 0;
    defaultTm.minute = 4;
    reset();
}

void TimerClass::displayTime(NixieDigits_s &timeDigits) {
    timeDigits.value[0] = (tm.minute / 1U) % 10;
    timeDigits.value[1] = (tm.minute / 10U) % 10;
    timeDigits.value[3] = (tm.hour / 1U) % 10;
    timeDigits.value[4] = (tm.hour / 10U) % 10;
}

void TimerClass::incrementSec(void) volatile {
    tm.incrementSec();
}

void TimerClass::decrementSec(void) volatile {
    tm.decrementSec();
    if (runningDown && tm.minute == 0 && tm.second == 0 && !alarm) {
        alarm = true;
        alarmTs = millis();
    }
}

void TimerClass::autoTurnoff(void) volatile {
    if ((millis() - alarmTs >= ALARM_TIMEOUT) && alarm) {
        alarm = false;
    }
}

void TimerClass::stopwatch(void) {
    defaultTm.copy(&tm);
    runningUp = true;
}

void TimerClass::loopHandler(void) {
    if (runningUp) {
        incrementSec();
    } else if (runningDown) {
        decrementSec();
    }
}

void TimerClass::start(void) {
    defaultTm.copy(&tm);
    runningDown = true;
}

void TimerClass::stop(void) {
    runningDown = false;
}

void TimerClass::resetAlarm(void) {
    if (alarm) {
        alarm = false;
        stop();
    }
}

void TimerClass::reset(void) {
    resetAlarm();
    runningDown = false;
    runningUp = false;
    tm.copy(&defaultTm);
}

void TimerClass::setTimeSlow(const char *var, const char *dir) {
    if (millis() - setTs > INCREMENT_TIME) {
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