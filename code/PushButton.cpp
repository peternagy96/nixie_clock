/*
* Class for the implementation of the right side push button
*/

#include "PushButton.h"
#include <Arduino.h>
#include <TimeLib.h>

#define BUTTON_LONG_PRESS_TIMEOUT 1000
#define BUTTON_REALLY_LONG_PRESS_TIMEOUT 20000

void PushButtonClass::setPin(uint8_t pin) {
    pinMode(pin, INPUT_PULLUP);
    this->pin = pin;
}

void PushButtonClass::readState(void) {
    if (digitalRead(pin) == LOW && !wasPressed) {
        press();
        longPressTs = millis();
        longPressed = true;
        reallyLongPressed = true;
        wasLongPressed = false;
        wasReallyLongPressed = false;

    } else if (digitalRead(pin) == HIGH && wasPressed) {
        release();
    }
}

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
        wasReallyLongPressed = false;
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

bool PushButtonClass::longPress(void) {
    bool rv = false;
    if (pressed && longPressed && millis() - longPressTs >= BUTTON_LONG_PRESS_TIMEOUT) {
        longPressed = false;
        wasLongPressed = true;
        rv = true;
    }
    return rv;
}

bool PushButtonClass::reallyLongPress(void) {
    bool rv = false;
    if (pressed && reallyLongPressed && millis() - longPressTs >= BUTTON_REALLY_LONG_PRESS_TIMEOUT) {
        reallyLongPressed = false;
        wasReallyLongPressed = true;
        rv = true;
    }
    return rv;
}