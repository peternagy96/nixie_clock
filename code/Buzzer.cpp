/*
* Buzzer class to implement buzzer features
*/

#include "Buzzer.h"
#include <stdint.h>
#include "Arduino.h"

BuzzerClass Buzzer;

void BuzzerClass::initialize(uint8_t buzzerPin) {
    this->buzzerPin = buzzerPin;
    pinMode(buzzerPin, OUTPUT);
    digitalWrite(buzzerPin, LOW);
    active = false;
    index = 0;
    initialized = true;
}

void BuzzerClass::readState(void) {
    uint32_t ts;

    if (!initialized || !active) return;

    ts = millis();

    if (ts - melodyTs > melody[index]) {
        digitalWrite(buzzerPin, !digitalRead(buzzerPin));
        melodyTs = ts;
        index++;
        if (melody[index] < 0) index = 0;
    }
}

void BuzzerClass::playMelody1(void) {
    if (!initialized || active) return;
    active = true;
    melody = melody1;
    melodyTs = millis() - 5000;
    index = 0;
}

void BuzzerClass::playMelody2(void) {
    if (!initialized || active) return;
    active = true;
    melody = melody2;
    melodyTs = millis() - 5000;
    index = 0;
}

void BuzzerClass::stop(void) {
    active = false;
    index = 0;
    digitalWrite(buzzerPin, LOW);
}