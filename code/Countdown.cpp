/*
* Timer class to implement countdown timer
*/

#include "Countdown.h"

#define ALARM_TIMEOUT 120000
#define INCREMENT_TIME 500

void CountdownClass::initialize(int dmin, int dsec) {
    defaultSec = dsec;
    defaultMin = dmin;
    sec = defaultSec;
    min = defaultMin;
}

void CountdownClass::displayTime(NixieDigits_s &timeDigits) {
    timeDigits.value[0] = (sec / 1U) % 10;
    timeDigits.value[1] = (sec / 10U) % 10;
    timeDigits.value[3] = (min / 1U) % 10;
    timeDigits.value[4] = (min / 10U) % 10;
}

void CountdownClass::incrementSec(void) volatile {
    sec++;
    if (sec > 59) {
        sec = 0;
        if (runningUp) {
            incrementMin();
        }
    }
}

void CountdownClass::decrementSec(void) volatile {
    sec--;
    if (sec == 0 && min == 0 && runningDown) {
        alarm = true;
        alarmTs = millis();
    } else if (sec < 0) {
        sec = 59;
        if (runningDown) {
            decrementMin();
        }
    }
}

void CountdownClass::incrementMin(void) volatile {
    min++;
    if (min > 99) {
        min = 0;
    }
}

void CountdownClass::decrementMin(void) volatile {
    min--;
    if (min < 0) {
        min = 99;
    }
}

void CountdownClass::autoTurnoff(void) volatile {
    if ((millis() - alarmTs >= ALARM_TIMEOUT) && alarm) {
        alarm = false;
        enabled = false;
    }
}

void CountdownClass::stopwatch(void) {
    if (!runningUp) {
        defaultSec = sec;
        defaultMin = min;
        sec = 0;
        min = 0;
        runningUp = true;
    }
}

void CountdownClass::loopHandler(void) {
    if (runningUp && !alarm && enabled) {
        incrementSec();
    } else if (runningDown && !alarm && enabled) {
        decrementSec();
    }
}

void CountdownClass::start(void) {
    if (!runningDown) {
        defaultSec = sec;
        defaultMin = min;
        runningDown = true;
    }
}

void CountdownClass::restart(void) {
    if (!runningDown) {
        runningDown = true;
    }
}

void CountdownClass::stop(void) {
    runningDown = false;
}

void CountdownClass::resetAlarm(void) {
    if (alarm) {
        alarm = false;
        stop();
    }
}

void CountdownClass::reset(void) {
    if (runningUp || runningDown) {
        resetAlarm();
        runningDown = false;
        runningUp = false;
        sec = defaultSec;
        min = defaultMin;
        enabled = true;
    }
}

void CountdownClass::resetTime(void) volatile {
    sec = defaultSec;
    min = defaultMin;
}

void CountdownClass::setDefault(void) volatile {
    defaultSec = sec;
    defaultMin = min;
}

void CountdownClass::setTimeSlow(const char *var, const char *dir) {
    if (millis() - setTs > INCREMENT_TIME) {
        if (var == "min") {
            if (dir == "+") {
                incrementMin();
            } else if (dir == "-") {
                decrementMin();
            }
        } else if (var == "sec") {
            if (dir == "+") {
                incrementSec();
            } else if (dir == "-") {
                decrementSec();
            }
        }
        setTs = millis();
    }
}