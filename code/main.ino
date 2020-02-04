/*
 * Nixie Clock main code
 * 
 * January 2020
 */

#include "Alarm.h"
#include "Buzzer.h"
#include "Nixie.h"
#include "PushButton.h"
#include "TiltSwitch.h"
#include "Timekeeper.h"
#include "Timer.h"

#include <Arduino.h>
#include <Wire.h>
#include "DS1302.h"
#include "avr/wdt.h"

// Variables
#define ANTIPOISING_DELAY 500  // anti poising delay time
#define SERIAL_BAUD 9600       // serial baud rate
#define WDT_TIMEOUT WDTO_4S    // timeout length of the watchdog timer

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

#define BUZZER_PIN 30
#define RTC_VCC_PIN 25

// analog pins
#define BUTTON0_APIN 6        // push button 0 - "mode"
#define BUTTON1_DOWN_APIN 11  // tilt button 1 - "increase"
#define BUTTON1_UP_APIN 10    // tilt button 1 - "decrease"
#define BUTTON2_DOWN_APIN 9   // tilt button 2 - "increase"
#define BUTTON2_UP_APIN 8     // tilt button 2 - "decrease"

// RTC pins
const byte RTC_EnablePin = A5;  //A5
const byte RTC_IOPin = A6;      //A6
const byte RTC_SerialPin = A7;  //A7
DS1302 rtc(RTC_EnablePin, RTC_IOPin, RTC_SerialPin);

// menu states

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
    NixieDigits_s dateDigits;           // stores the Nixie display digit values of the current date
    bool button0State;
    uint8_t switch0State;
    uint8_t switch1State;  // ToDo: store nixie tube display numbers
} G_t;

G_t G;
Time systemTm(2020, 1, 1, 12, 34, 0, Time::kMonday);

// create objects
PushButtonClass PushButton;
TiltSwitchClass TiltSwitch[2];
AlarmClass Alarm;
TimerClass Timer;
TimekeeperClass Timekeeper;

void setup() {
    wdt_disable();  // and disable watchdog

    rtc.writeProtect(false);
    rtc.halt(false);

#ifndef SERIAL_DEBUG
    Serial.begin(SERIAL_BAUD);
#endif

    Serial.println(" ");
    Serial.println("+ + +  N I X I E  C L O C K  + + +");
    Serial.println(" ");

    //delay(3000); // wait for console opening
    G.timeDigits.value[0] = (systemTm.min / 1U) % 10;
    G.timeDigits.value[1] = (systemTm.min / 10U) % 10;
    G.timeDigits.value[3] = (systemTm.hr / 1U) % 10;
    G.timeDigits.value[4] = (systemTm.hr / 10U) % 10;

    // initialize the nixie tubes
    Nixie.initialize(ANODE0_PIN, ANODE1_PIN, ANODE2_PIN, ANODE3_PIN, ANODE4_PIN,
                     BCD0_PIN, BCD1_PIN, BCD2_PIN, BCD3_PIN, &G.timeDigits);

    // initialize the timekeeper
    //Timekeeper.initialize();
    PushButton.setPin(BUTTON0_APIN);
    TiltSwitch[0].setPin(BUTTON1_UP_APIN, BUTTON1_DOWN_APIN);
    TiltSwitch[1].setPin(BUTTON2_UP_APIN, BUTTON2_DOWN_APIN);

    //if time on RTC is not the initial time, then do nothing,
    // otherwise load default systemTm go into SET_TIME mode
    rtc.time(systemTm);

    // enable the watchdog
    wdt_enable(WDT_TIMEOUT);
}

void loop() {
    // get current time from RTC
    systemTm = rtc.time();  // ToDo: implement it into the chrono class

    getButtonStates();

    Nixie.refresh();  // refresh method is called many times across the code to ensure smooth display operation

    //setDisplay();  // navigate the settings menu
    if (TiltSwitch[0].up || TiltSwitch[1].down) {
        Nixie.enable(false);
    } else if (!PushButton.rising()) {
        Nixie.enable(true);
    }

    if (PushButton.rising()) {
        if (Nixie.enabled) {
            Nixie.enable(false);
        } else {
            Nixie.enable(true);
        }
    }

    Nixie.refresh();
}

void getButtonStates(void) {
    PushButton.readState();
    TiltSwitch[0].readState();
    TiltSwitch[1].readState();
}

void setDisplay(void) {
    // logic to go to the proper current state
    // -> set next state depending on button states

    if (PushButton.rising()) {
        switch (G.menuState) {
            case SHOW_TIME:
                basicTiltMode();

                // TO NEXT STATE
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

            case SET_HOUR:
                if (TiltSwitch[1].up) {
                    // ! increase hours every 1/4 secs
                } else if (TiltSwitch[1].down) {
                    // ! decrease hours every 1/4 secs
                }
            case SHOW_TIMER:
                if (TiltSwitch[1].up || TiltSwitch[1].down) {
                    // ! enable timer
                }

                // TO NEXT STATES
                if (PushButton.falling()) {
                    G.menuState = SHOW_TIME;
                } else if (PushButton.longPress()) {
                    G.menuState = SET_TIMER_MIN;
                }

            case SET_TIMER_MIN:
                if (TiltSwitch[1].up) {
                    // ! increase timer mins
                } else if (TiltSwitch[1].down) {
                    // ! decrease timer mins
                }

                // TO NEXT STATE
                if (PushButton.falling()) {
                    G.menuState = SET_TIMER_SEC;
                }

            case SET_TIMER_SEC:
                // ! blink seconds tubes
                if (TiltSwitch[1].up) {
                    // ! increase timer secs
                } else if (TiltSwitch[1].down) {
                    // ! decrease timer secs
                }
                // TO NEXT STATE
                if (PushButton.falling()) {
                    G.menuState = SHOW_TIMER;
                }
            case SHOW_DATE:
                basicTiltMode();
                // TO NEXT STATE
                if (PushButton.falling()) {
                    G.menuState = SHOW_TIMER;
                } else if (PushButton.longPress()) {
                    G.menuState = SET_MONTH;
                }
            case SHOW_YEAR:
                basicTiltMode();
                // TO NEXT STATE
                if (PushButton.falling()) {
                    G.menuState = SHOW_TIMER;
                } else if (PushButton.longPress()) {
                    G.menuState = SET_MONTH;
                }
            case SET_MONTH:
                // ! blink the month tubes
                if (PushButton.falling()) {
                    G.menuState = SET_DAY;
                }
            case SET_DAY:
                if (PushButton.falling()) {
                    G.menuState = SET_YEAR;
                }
            case SET_YEAR:
                if (PushButton.falling()) {
                    G.menuState = SHOW_DATE;
                }
            case SHOW_ALARM1:
            // ! show alarm time for a second when toggle switch is set, then switch to show time
            case SET_ALARM1_HOUR:
                if (PushButton.falling()) {
                    G.menuState = SET_ALARM1_MIN;
                }
            case SET_ALARM1_MIN:
                if (PushButton.falling()) {
                    G.menuState = SHOW_ALARM1;
                }
            case SHOW_ALARM2:
            // ! show alarm time for a second when toggle switch is set, then switch to show time
            case SET_ALARM2_HOUR:
                if (PushButton.falling()) {
                    G.menuState = SET_ALARM2_MIN;
                }
            case SET_ALARM2_MIN:
                if (PushButton.falling()) {
                    G.menuState = SHOW_ALARM2;
                }
        }
    }
}

void tiltMode(void) {
    /* switch (G.menuState) {
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
    }*/
}

void basicTiltMode(void) {
    if (TiltSwitch[0].up) {
        // ! enable alarm 1
    } else if (TiltSwitch[0].down) {
        // ! enable alarm 2
    } else {
        // ! disable bpth alarms
    }

    if (TiltSwitch[1].up) {
        // ! disable nixie display
    } else if (TiltSwitch[1].down) {
        // ! enable nixie display
        // ! disable power saving mode
    } else {
        // ! enable power saving mode
    }
}

void updateDigits() {
    static int8_t lastMin = 0;

    // check whether current state requires time or date display
    G.timeDigits.value[0] = dec2bcdLow(systemTm.min);
    G.timeDigits.value[1] = dec2bcdHigh(systemTm.min);
    G.timeDigits.value[2] = dec2bcdLow(systemTm.hr);
    G.timeDigits.value[3] = dec2bcdHigh(systemTm.hr);
    /*G.dateDigits.value[0] = dec2bcdLow  (G.systemTm->tm_year);
  G.dateDigits.value[1] = dec2bcdHigh (G.systemTm->tm_year);
  G.dateDigits.value[2] = dec2bcdLow  (G.systemTm->tm_mon + 1);
  G.dateDigits.value[3] = dec2bcdHigh (G.systemTm->tm_mon + 1);
  G.dateDigits.value[4] = dec2bcdLow  (G.systemTm->tm_mday);
  G.dateDigits.value[5] = dec2bcdHigh (G.systemTm->tm_mday); */

    if (G.menuState == SHOW_TIME) {
        // trigger Nixie digit "Slot Machine" effect
        if (systemTm.min != lastMin && (G.timeDigits.value[2] == 0)) {
            Nixie.slotMachine();
        }
        // trigger the cathode poisoning prevention routine
        if (G.timeDigits.value[0] == 0) {
            Nixie.cathodePoisonPrevent();
        }
    }
    lastMin = systemTm.min;
}

uint8_t dec2bcdLow(uint8_t value) {
    while (value >= 10) value -= 10;
    return value;
}

uint8_t dec2bcdHigh(uint8_t value) {
    uint8_t rv = 0;
    while (value >= 100) value -= 100;
    while (value >= 10) {
        value -= 10;
        rv++;
    }
    return rv;
}