/*
* Class for the implementation of the tilt switches
*/

#include "TiltSwitch.h"

void TiltSwitchClass::setUp(void) {
    up = true;
    down = false;
    middle = false;
}

void TiltSwitchClass::setDown(void) {
    up = false;
    down = true;
    middle = false;
}

void TiltSwitchClass::setMiddle(void) {
    up = false;
    down = false;
    middle = true;
}

void TiltSwitchClass::incrementRight(ChronoClass* chrono) {
    chrono->incrementRight()
}

void TiltSwitchClass::decrementRight(ChronoClass* chrono) {
    chrono->decrementRight()
}

void TiltSwitchClass::incrementLeft(ChronoClass* chrono) {
    chrono->incrementLeft()
}

void TiltSwitchClass::decrementLeft(ChronoClass* chrono) {
    chrono->decrementLeft()
}
