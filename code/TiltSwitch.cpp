/*
* Class for the implementation of the tilt switches
*/

#include "TiltSwitch.h"

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
