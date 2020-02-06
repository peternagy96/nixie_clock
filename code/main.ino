/*
 * Nixie Clock main code
 * 
 * January 2020
 */

#include "Alarm.h"
#include "Buzzer.h"
#include "Countdown.h"
#include "Nixie.h"
#include "PushButton.h"
#include "TiltSwitch.h"
#include "Timekeeper.h"

#include <Arduino.h>
#include <Wire.h>
#include "DS1302.h"
#include "avr/wdt.h"

// Variables
#define ANTIPOISING_DELAY 500  // anti poising delay time
#define SERIAL_BAUD 9600       // serial baud rate
#define WDT_TIMEOUT WDTO_4S    // timeout length of the watchdog timer
#define SECOND 1000            // 1000

// Pin variables
#define ANODE0_PIN 14  //A4 1MIN
#define ANODE1_PIN 15  //A3 10MIN
#define ANODE2_PIN 16  //   COMMA
#define ANODE3_PIN 17  //A1 1HR
#define ANODE4_PIN 18  //A0 10HR

#define BCD0_PIN 5
#define BCD1_PIN 4
#define BCD2_PIN 3
#define BCD3_PIN 2

#define BUZZER_PIN 12
#define RTC_VCC_PIN 7

// button pins
#define BUTTON0_APIN 6        // push button 0 - "mode"
#define BUTTON1_DOWN_APIN 11  // tilt button 1 - "increase"
#define BUTTON1_UP_APIN 10    // tilt button 1 - "decrease"
#define BUTTON2_DOWN_APIN 9   // tilt button 2 - "increase"
#define BUTTON2_UP_APIN 8     // tilt button 2 - "decrease"

// RTC pins
const byte RTC_EnablePin = A5;  //A5
const byte RTC_IOPin = A6;      //A6 //! cannot be a digital input pin
const byte RTC_SerialPin = A7;  //A7 //! cannot be a digital input pin
//DS1302 rtc(RTC_EnablePin, RTC_IOPin, RTC_SerialPin);

enum MenuState_e {
    SHOW_TIME,
    SET_HOUR,
    SET_MIN,
    SHOW_TIMER,
    SET_TIMER_MIN,
    SET_TIMER_SEC,
    SHOW_DATE,
    SHOW_YEAR,
    SET_MONTH,
    SET_DAY,
    SET_YEAR,
    SHOW_ALARM1,
    SET_ALARM1_HOUR,
    SET_ALARM1_MIN,
    SHOW_ALARM2,
    SET_ALARM2_HOUR,
    SET_ALARM2_MIN
};

/*
 * Global variables
 */
typedef struct {
    bool manuallyAdjusted = true;       // prevent crystal drift compensation if clock was manually adjusted
    MenuState_e menuState = SHOW_TIME;  // stores the state in the menu state machine
    NixieDigits_s timeDigits;           // stores the Nixie display digit values of the current time
    bool button0State;
    uint8_t switch0State;
    uint8_t switch1State;  // ToDo: store nixie tube display numbers
    uint32_t secTs = 0;
    bool secFlag = true;
    bool twoSecFlag = true;
} G_t;

G_t G;
//Time systemTm = rtc.time();
Time systemTm(2020, 1, 1, 0, 0, 0, Time::kMonday);

// create objects
PushButtonClass PushButton;
TiltSwitchClass TiltSwitch[2];
AlarmClass Alarm[2];
CountdownClass Countdown;

void setup() {
    wdt_disable();  // and disable watchdog

    //Serial.begin(SERIAL_BAUD);

    Nixie.initialize(ANODE0_PIN, ANODE1_PIN, ANODE2_PIN, ANODE3_PIN, ANODE4_PIN,
                     BCD0_PIN, BCD1_PIN, BCD2_PIN, BCD3_PIN, &G.timeDigits);

    Timekeeper.initialize(&systemTm);
    Alarm[0].initialize();
    Alarm[1].initialize();
    Countdown.initialize();
    Buzzer.initialize(BUZZER_PIN);

    PushButton.setPin(BUTTON0_APIN);
    TiltSwitch[0].setPin(BUTTON1_UP_APIN, BUTTON1_DOWN_APIN);
    TiltSwitch[1].setPin(BUTTON2_UP_APIN, BUTTON2_DOWN_APIN);

    //if time on RTC is not the initial time, then do nothing,
    // otherwise load default systemTm go into SET_TIME mode
    //pinMode(RTC_VCC_PIN, OUTPUT);
    //digitalWrite(RTC_VCC_PIN, HIGH);
    //rtc.writeProtect(false);
    //rtc.halt(false);

    //rtc.time(defaultTm);

    if (systemTm.hr == 0 && systemTm.mon == 1) {
        G.menuState = SHOW_TIME;
    }

    // enable the watchdog
    wdt_enable(WDT_TIMEOUT);
}

void loop() {
    //systemTm = defaultTm;  //rtc.time();  // ToDo: implement it into the chrono class

    if (G.secFlag) {
        G.twoSecFlag = !G.twoSecFlag;
        G.secFlag = false;
        G.secTs = millis();
        Timekeeper.incrementSec();
        Countdown.loopHandler();
    }

    // ToDo: implement function to run once a day

    Nixie.refresh();  // refresh method is called many times across the code to ensure smooth display operation

    getButtonStates();

    Nixie.refresh();

    Buzzer.readState();

    Nixie.refresh();

    alarmLoophandler();

    Nixie.refresh();

    navigateMenu();

    Nixie.refresh();

    if (millis() - G.secTs >= SECOND) {
        G.secFlag = true;
    }

    displayMenu();

    Nixie.refresh();
}

void getButtonStates(void) {
    PushButton.readState();
    TiltSwitch[0].readState();
    TiltSwitch[1].readState();
}

void alarmLoophandler(void) {
    if (TiltSwitch[0].up) {
        Alarm[0].alarmGoesOff();
    } else if (TiltSwitch[0].down) {
        Alarm[1].alarmGoesOff();
    }
    Alarm[0].autoTurnoff();
    Alarm[1].autoTurnoff();
    Countdown.autoTurnoff();
}

void displayMenu(void) {
    // show the proper digits and setup proper blinking
    // depending on the tilt switch states increment/decrement variables
    switch (G.menuState) {
        case SHOW_TIME:
            Nixie.blinkNone();
            Timekeeper.displayTime(G.timeDigits);
            Timekeeper.isBeingSet = false;
            if (Timekeeper.wasSet) {
                Timekeeper.setSecToNull();
            }

            break;
        case SET_HOUR:
            Timekeeper.displayTime(G.timeDigits);
            Timekeeper.isBeingSet = true;
            if (TiltSwitch[1].up) {
                Nixie.blinkNone();
                Timekeeper.wasSet = true;
                Timekeeper.setTimeSlow("hour", "+");
            } else if (TiltSwitch[1].down) {
                Nixie.blinkNone();
                Timekeeper.wasSet = true;
                Timekeeper.setTimeSlow("hour", "-");
            } else {
                Nixie.blinkLeft();
            }
            break;
        case SET_MIN:
            Timekeeper.displayTime(G.timeDigits);
            if (TiltSwitch[1].up) {
                Nixie.blinkNone();
                Timekeeper.wasSet = true;
                Timekeeper.setTimeSlow("min", "+");
            } else if (TiltSwitch[1].down) {
                Nixie.blinkNone();
                Timekeeper.wasSet = true;
                Timekeeper.setTimeSlow("min", "-");
            } else {
                Nixie.blinkRight();
            }
            break;
        case SHOW_TIMER:
            Countdown.displayTime(G.timeDigits);
            Nixie.blinkNone();
            if (TiltSwitch[1].up) {
                Countdown.stopwatch();
            } else if (TiltSwitch[1].down) {
                Countdown.start();
            } else {
                Countdown.reset();
            }

            break;
        case SET_TIMER_MIN:
            Countdown.displayTime(G.timeDigits);
            if (TiltSwitch[1].up) {
                Nixie.blinkNone();
                Countdown.setTimeSlow("min", "+");
            } else if (TiltSwitch[1].down) {
                Nixie.blinkNone();
                Countdown.setTimeSlow("min", "-");
            } else {
                Nixie.blinkLeft();
            }
            break;
        case SET_TIMER_SEC:
            Countdown.displayTime(G.timeDigits);
            if (TiltSwitch[1].up) {
                Nixie.blinkNone();
                Countdown.setTimeSlow("sec", "+");
            } else if (TiltSwitch[1].down) {
                Nixie.blinkNone();
                Countdown.setTimeSlow("sec", "-");
            } else {
                Nixie.blinkRight();
            }
            break;
        case SHOW_DATE:
            Timekeeper.displayDate(G.timeDigits);
            Nixie.blinkNone();
            if (G.secFlag) {
                G.menuState = SHOW_YEAR;
            }
            break;
        case SHOW_YEAR:
            Timekeeper.displayYear(G.timeDigits);
            Nixie.blinkNone();
            if (G.twoSecFlag) {
                G.menuState = SHOW_DATE;
            }
            break;
        case SET_MONTH:
            Timekeeper.displayDate(G.timeDigits);
            if (TiltSwitch[1].up) {
                Nixie.blinkNone();
                Timekeeper.setTimeSlow("month", "+");
            } else if (TiltSwitch[1].down) {
                Nixie.blinkNone();
                Timekeeper.setTimeSlow("month", "-");
            } else {
                Nixie.blinkLeft();
            }
            break;
        case SET_DAY:
            Timekeeper.displayDate(G.timeDigits);
            if (TiltSwitch[1].up) {
                Nixie.blinkNone();
                Timekeeper.setTimeSlow("date", "+");
            } else if (TiltSwitch[1].down) {
                Nixie.blinkNone();
                Timekeeper.setTimeSlow("date", "-");
            } else {
                Nixie.blinkRight();
            }
            break;
        case SET_YEAR:
            Timekeeper.displayYear(G.timeDigits);
            if (TiltSwitch[1].up) {
                Nixie.blinkNone();
                Timekeeper.setTimeSlow("year", "+");
            } else if (TiltSwitch[1].down) {
                Nixie.blinkNone();
                Timekeeper.setTimeSlow("year", "-");
            } else {
                Nixie.blinkAll();
            }
            break;
        case SHOW_ALARM1:
            Alarm[0].displayTime(G.timeDigits);
            if (G.twoSecFlag) {
                G.menuState = SHOW_TIME;
            }
            break;
        case SET_ALARM1_HOUR:
            Alarm[0].displayTime(G.timeDigits);
            if (TiltSwitch[1].up) {
                Nixie.blinkNone();
                Alarm[0].setTimeSlow("hour", "+");
            } else if (TiltSwitch[1].down) {
                Nixie.blinkNone();
                Alarm[0].setTimeSlow("hour", "-");
            } else {
                Nixie.blinkLeft();
            }
            break;
        case SET_ALARM1_MIN:
            Alarm[0].displayTime(G.timeDigits);
            if (TiltSwitch[1].up) {
                Nixie.blinkNone();
                Alarm[0].setTimeSlow("min", "+");
            } else if (TiltSwitch[1].down) {
                Nixie.blinkNone();
                Alarm[0].setTimeSlow("min", "-");
            } else {
                Nixie.blinkRight();
            }
            break;
        case SHOW_ALARM2:
            Alarm[1].displayTime(G.timeDigits);
            if (G.twoSecFlag) {
                G.menuState = SHOW_TIME;
            }
            break;
        case SET_ALARM2_HOUR:
            Alarm[1].displayTime(G.timeDigits);
            if (TiltSwitch[1].up) {
                Nixie.blinkNone();
                Alarm[1].setTimeSlow("hour", "+");
            } else if (TiltSwitch[1].down) {
                Nixie.blinkNone();
                Alarm[1].setTimeSlow("hour", "-");
            } else {
                Nixie.blinkLeft();
            }
            break;
        case SET_ALARM2_MIN:
            Alarm[1].displayTime(G.timeDigits);
            if (TiltSwitch[1].up) {
                Nixie.blinkNone();
                Alarm[1].setTimeSlow("min", "+");
            } else if (TiltSwitch[1].down) {
                Nixie.blinkNone();
                Alarm[1].setTimeSlow("min", "-");
            } else {
                Nixie.blinkRight();
            }
            break;
    }
}

void navigateMenu(void) {
    // -> set next state depending on button states

    switch (G.menuState) {
        case SHOW_TIME:
            if (PushButton.falling()) {
                G.menuState = SHOW_DATE;
                // ! initialize dateshow Timer
            } else if (PushButton.longPress() && TiltSwitch[0].middle) {
                G.menuState = SET_HOUR;
            } else if (PushButton.longPress() && TiltSwitch[0].up) {
                G.menuState = SET_ALARM1_HOUR;
            } else if (PushButton.longPress() && TiltSwitch[0].down) {
                G.menuState = SET_ALARM2_HOUR;
            }
            break;
        case SET_HOUR:
            if (PushButton.falling()) {
                G.menuState = SET_MIN;
            }
            break;
        case SET_MIN:
            if (PushButton.falling()) {
                G.menuState = SHOW_TIME;
            }
            break;
        case SHOW_TIMER:
            if (PushButton.falling()) {
                G.menuState = SHOW_TIME;
            } else if (PushButton.longPress()) {
                G.menuState = SET_TIMER_MIN;
            }
            break;
        case SET_TIMER_MIN:
            if (PushButton.falling()) {
                G.menuState = SET_TIMER_SEC;
            }
            break;
        case SET_TIMER_SEC:
            if (PushButton.falling()) {
                G.menuState = SHOW_TIMER;
            }
            break;
        case SHOW_DATE:
            Nixie.blinkNone();
            if (PushButton.falling()) {
                G.menuState = SHOW_TIMER;
            } else if (PushButton.longPress()) {
                G.menuState = SET_MONTH;
            }
            break;
        case SHOW_YEAR:
            Nixie.blinkNone();
            if (PushButton.falling()) {
                G.menuState = SHOW_TIMER;
            } else if (PushButton.longPress()) {
                G.menuState = SET_MONTH;
            }
            break;
        case SET_MONTH:
            if (PushButton.falling()) {
                G.menuState = SET_DAY;
            }
            break;
        case SET_DAY:
            if (PushButton.falling()) {
                G.menuState = SET_YEAR;
            }
            break;
        case SET_YEAR:
            if (PushButton.falling()) {
                G.menuState = SHOW_DATE;
            }
            break;
        case SHOW_ALARM1:
            break;
        case SET_ALARM1_HOUR:
            if (PushButton.falling()) {
                G.menuState = SET_ALARM1_MIN;
            }
            break;
        case SET_ALARM1_MIN:
            if (PushButton.falling()) {
                G.menuState = SHOW_ALARM1;
            }
            break;
        case SHOW_ALARM2:
            break;
        case SET_ALARM2_HOUR:
            if (PushButton.falling()) {
                G.menuState = SET_ALARM2_MIN;
            }
            break;
        case SET_ALARM2_MIN:
            if (PushButton.falling()) {
                G.menuState = SHOW_ALARM2;
            }
            break;
    }
}
