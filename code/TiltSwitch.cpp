/*
* Class for the implementation of the tilt switches
*/

#include "TiltSwitch.h"
#include <Arduino.h>

void TiltSwitchClass::setPin(uint8_t upPin, uint8_t downPin) {
    pinMode(upPin, INPUT);
    this->upPin = upPin;
    pinMode(downPin, INPUT);
    this->downPin = downPin;
}

void TiltSwitchClass::readState(void) {
    if (digitalRead(upPin) == LOW) {
        setToUp();
    } else if (digitalRead(downPin) == LOW) {
        setToDown();
    } else {
        setToMiddle();
    }
}

void TiltSwitchClass::setToUp(void) {
    up = true;
    down = false;
    middle = false;
}

void TiltSwitchClass::setToDown(void) {
    up = false;
    down = true;
    middle = false;
}

void TiltSwitchClass::setToMiddle(void) {
    up = false;
    down = false;
    middle = true;
}

void TiltSwitchClass::incrementRight(ChronoClass* chrono) {
    chrono->incrementRight();
}

void TiltSwitchClass::decrementRight(ChronoClass* chrono) {
    chrono->decrementRight();
}

void TiltSwitchClass::incrementLeft(ChronoClass* chrono) {
    chrono->incrementLeft();
}

void TiltSwitchClass::decrementLeft(ChronoClass* chrono) {
    chrono->decrementLeft();
}
