/*
* Class for the implementation of the tilt switches
*/

#include "TiltSwitch.h"

void TiltSwitchClass::setUp(void)
{
    up = true;
    down = false;
    middle = false;
}

void TiltSwitchClass::setDown(void)
{
    up = false;
    down = true;
    middle = false;
}

void TiltSwitchClass::setMiddle(void)
{
    up = false;
    down = false;
    middle = true;
}
