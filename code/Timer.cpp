/*
* Timer class to implement countdown timer
*/

#include "Timer.h"

void TimerClass::initialize(void (*callback)(bool)) {
    defaultTm.second = 0;
    defaultTm.minute = 4;
    reset();
}

void TimerClass::incrementRight(void) {
    tm.incrementRight();
}

void TimerClass::decrementRight(void) {
    tm.decrementRight();
    if (running && alarmGoesOff()) {
        alarm = true
    }
}

void alarmGoesOff(void) {
    if (tm.minute == 0 && tm.hour == 0) {
        alarm = true
    }
}

void TimerClass::incrementLeft(void) {
    tm.incrementLeft();
}

void TimerClass::decrementLeft(void) {
    tm.decrementLeft();
}

void TimerClass::start(void) {
    defaultTm.copy(&tm);
    active = true;
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