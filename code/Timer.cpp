/*
* Timer class to implement countdown timer
*/

#include "Timer.h"

void TimerClass::initialize() {
    defaultTm.second = 0;
    defaultTm.minute = 4;
    reset();
}

void TimerClass::incrementSec(void) volatile {  // ? does this increment mins ir secs
    tm.incrementSec();
}

void TimerClass::decrementSec(void) volatile {
    tm.decrementSec();
    if (running && alarmGoesOff()) {
        alarm = true;
    }
}

bool TimerClass::alarmGoesOff(void) volatile {
    if (tm.minute == 0 && tm.hour == 0) {
        return true;
    }
    return false;
}

void TimerClass::incrementMin(void) volatile {
    tm.incrementMin();
}

void TimerClass::decrementMin(void) volatile {
    tm.decrementMin();
}

void TimerClass::incrementHour(void) volatile {
    tm.incrementHour();
}

void TimerClass::decrementHour(void) volatile {
    tm.decrementHour();
}

void TimerClass::start(void) {
    defaultTm.copy(&tm);
    running = true;
}

void TimerClass::stop(void) {
    running = false;
}

void TimerClass::resetAlarm(void) {
    if (alarm) {
        alarm = false;
        stop();
    }
}

void TimerClass::reset(void) {
    resetAlarm();
    running = false;
    tm.copy(&defaultTm);
}