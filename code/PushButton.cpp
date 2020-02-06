/*
* Class for the implementation of the right side push button
*/

#include "PushButton.h"
#include <Arduino.h>
#include <Time.h>

#define BUTTON_LONG_PRESS_TIMEOUT 1000

void PushButtonClass::setPin(uint8_t pin) {
    pinMode(pin, INPUT);
    this->pin = pin;
}

void PushButtonClass::readState(void) {
    if (digitalRead(pin) == LOW && !wasPressed) {
        press();
        longPressTs = millis();
        longPressed = true;
        wasLongPressed = false;

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