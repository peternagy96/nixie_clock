/*
* Timer class to implement countdown timer
*/

#include "Timer.h"

void TimerClass::initialize() {
    defaultTm.second = 0;
    defaultTm.minute = 4;
    reset();
}

void TimerClass::incrementRight(void) volatile {  // ? does this increment mins ir secs
    tm.incrementRight();
}

void TimerClass::decrementRight(void) volatile {
    tm.decrementRight();
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

void TimerClass::incrementLeft(void) volatile {
    tm.incrementLeft();
}

void TimerClass::decrementLeft(void) volatile {
    tm.decrementLeft();
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
    defaultTm.roundup();
    tm.copy(&defaultTm);
}