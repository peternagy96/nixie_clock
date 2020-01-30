/*
* Class for the implementation of the right side push button
*/

#include "PushButton.h"
#include <Arduino.h>
#include <Time.h>

#define BUTTON_LONG_PRESS_TIMEOUT (1000)

void PushButtonClass::press(void) {
    wasPressed = pressed;
    pressed = true;
}

void PushButtonClass::release(void) {
    wasPressed = pressed;
    pressed = false;
}

bool PushButtonClass::rising(void) {
    bool rv = false;
    if (pressed && !wasPressed) {
        wasPressed = pressed;
        longPressTs = millis();
        longPressed = true;
        wasLongPressed = false;
        rv = true;
    }
    return rv;
}

bool PushButtonClass::falling(void) {
    bool rv = false;
    if (!pressed && wasPressed && !wasLongPressed) {
        wasPressed = pressed;
        rv = true;
    }
    return rv;
}

bool PushButtonClass::fallingLongPress(void) {
    bool rv = false;
    if (!pressed && wasPressed && wasLongPressed) {
        wasPressed = pressed;
        wasLongPressed = false;
        rv = true;
    }
    return rv;
}

bool PushButtonClass::fallingContinuous(void) {
    return !pressed && wasPressed;
}

bool PushButtonClass::longPress(void) {
    bool rv = false;
    if (pressed && longPressed && millis() - longPressTs > BUTTON_LONG_PRESS_TIMEOUT) {
        longPressed = false;
        wasLongPressed = true;
        rv = true;
    }
    return rv;
}