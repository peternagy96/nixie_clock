/*
 * Tests to validate single components on the board
 * 
 * January 2020
 */

#include "C:\Users\peter\Documents\Code\Nixie clock\code\Alarm.h"
#include "C:\Users\peter\Documents\Code\Nixie clock\code\Buzzer.h"
#include "C:\Users\peter\Documents\Code\Nixie clock\code\Nixie.h"
#include "C:\Users\peter\Documents\Code\Nixie clock\code\PushButton.h"
#include "C:\Users\peter\Documents\Code\Nixie clock\code\TiltSwitch.h"
#include "C:\Users\peter\Documents\Code\Nixie clock\code\Timekeeper.h"
#include "C:\Users\peter\Documents\Code\Nixie clock\code\Timer.h"

#include <Arduino.h>
#include <Wire.h>
#include "DS1302.h"
#include "avr/wdt.h"

#define TEST_TYPE 0

/*  tubes:              0
    rtc (correct time): 1
    multiplexing:       2
    blinking:           3
    buttons:            4
    buzzer:             5
*/

// Variables
#define MULTIPLEX_DELAY 2      // multiplex delay time
#define ANTIPOISING_DELAY 500  // anti poising delay time
#define SERIAL_BAUD 9600       // serial baud rate
#define WDT_TIMEOUT WDTO_4S    // timeout length of the watchdog timer

// Pin variables
#define ANODE0_PIN 18  //A4 1MIN
#define ANODE1_PIN 17  //A3 10MIN
#define ANODE2_PIN 15  //A1 1HR
#define ANODE3_PIN 14  //A0 10HR

#define BCD0_PIN 5
#define BCD1_PIN 4
#define BCD2_PIN 3
#define BCD3_PIN 2

#define COMMA_PIN 16
#define BUZZER_PIN 12

// analog pins
#define BUTTON0_APIN 13        // push button 0 - "mode"
#define BUTTON1_UP_APIN 11     // tilt button 1 - "increase"
#define BUTTON1__DOWN_APIN 10  // tilt button 1 - "decrease"
#define BUTTON2_UP_APIN 9      // tilt button 2 - "increase"
#define BUTTON2_DOWN_APIN 8    // tilt button 2 - "decrease"

// RTC pins
#define RTC_EnablePin 21  //A7 - A5
#define RTC_IOPin 20
#define RTC_SerialPin 19
//DS1302 rtc(RTC_EnablePin, RTC_IOPin, RTC_SerialPin);

/*
 * Global variables
 */
typedef struct {
    bool manuallyAdjusted = true;  // prevent crystal drift compensation if clock was manually adjusted
    NixieDigits_s timeDigits;      // stores the Nixie display digit values of the current time
    NixieDigits_s dateDigits;      // stores the Nixie display digit values of the current date
    bool button0State;
    uint8_t switch0State;
    uint8_t switch1State;  // ToDo: store nixie tube display numbers
} G_t;

G_t G;
//Time systemTm(2020, 1, 1, 0, 0, 0, Time::kMonday);

// create objects
//PushButtonClass PushButton;
//TiltSwitchClass TiltSwitch[2];
//AlarmClass Alarm;
//TimerClass Timer;
//TimekeeperClass Timekeeper;

uint8_t anodePin[4] = {ANODE0_PIN, ANODE1_PIN, ANODE2_PIN, ANODE3_PIN};
uint8_t bcdPin[4] = {BCD0_PIN, BCD1_PIN, BCD2_PIN, BCD3_PIN};

void setup() {
#ifndef SERIAL_DEBUG
    Serial.begin(SERIAL_BAUD);
#endif

    Serial.println(" ");
    Serial.println("+ + +  N I X I E  C L O C K  TESTS  + + +");
    Serial.println(" ");

    switch (TEST_TYPE) {
        case (0):
            for (int i = 0; i < 4; i++) {
                pinMode(anodePin[i], OUTPUT);
                digitalWrite(anodePin[i], LOW);
            }
            for (int i = 0; i < 4; i++) {
                pinMode(bcdPin[i], OUTPUT);
                digitalWrite(bcdPin[i], LOW);
            }
            pinMode(COMMA_PIN, OUTPUT);
            digitalWrite(COMMA_PIN, LOW);
    }
}

void loop() {
    switch (TEST_TYPE) {
        case (0):
        for (int j = 0; j < 4; j++) {
            for (int i = 0; i < 10; i++) {
                digitalWrite(anodePin[j], HIGH);
                digitalWrite(bcdPin[3], i & 1);
                digitalWrite(bcdPin[2], (i >> 1) & 1);
                digitalWrite(bcdPin[1], (i >> 2) & 1);
                digitalWrite(bcdPin[0], (i >> 3) & 1);
                delay(500);
                digitalWrite(anodePin[j], LOW);
            }
        }
    }
}
