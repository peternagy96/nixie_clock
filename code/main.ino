/*
 * Nixie Clock main code
 * 
 * January 2020
 */

#include "Buzzer.h"
//#include "Helper.h"
#include "Nixie.h"
#include "PushButton.h"
#include "TiltSwitch.h"
#include "Timekeeper.h"
#include "Timer.h"

#include <Arduino.h>
#include <Wire.h>
#include "RTClib/DS1302.h"
#include "avr/wdt.h"

// Variables
#define MULTIPLEX_DELAY 2      // multiplex delay time
#define ANTIPOISING_DELAY 500  // anti poising delay time
#define SERIAL_BAUD 9600       // serial baud rate
#define WDT_TIMEOUT            // timeout length of the watchdog timer

// Pin variables
#define ANODE0_PIN 12
#define ANODE1_PIN 11
#define ANODE2_PIN 10
#define ANODE3_PIN 7

#define BCD0_PIN 9
#define BCD1_PIN 6
#define BCD2_PIN 5
#define BCD3_PIN 8

#define COMMA_PIN 13

// analog pins
#define BUTTON0_APIN A2        // push button 0 - "mode"
#define BUTTON1_UP_APIN A3     // tilt button 1 - "increase"
#define BUTTON1__DOWN_APIN A1  // tilt button 1 - "decrease"
#define BUTTON2_UP_APIN A3     // tilt button 2 - "increase"
#define BUTTON2_DOWN_APIN A1   // tilt button 2 - "decrease"

// RTC pins
#define RTC_EnablePin 5
#define RTC_IOPin 6
#define RTC_SerialPin 7
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
    bool button0State;
    uint8_t switch0State;
    uint8_t switch1State;  // ToDo: store nixie tube display numbers
} G_t;

G_t G;

// create objects
PushButtonClass PushButton;
TiltSwitchClass TiltSwitch[2];
AlarmClass Alarm;
TimerClass Timer;
TimekeeperClass Timekeeper;

void setup() {
    time_t sysTime;
    MCUSR = 0;      // clear MCU status register
    wdt_disable();  // and disable watchdog

#ifndef SERIAL_DEBUG
    Serial.begin(SERIAL_BAUD);
#endif

    Serial.println(" ");
    Serial.println("+ + +  N I X I E  C L O C K  + + +");
    Serial.println(" ");

    //delay(3000); // wait for console opening

    // initialize the nixie tubes
    Nixie.initialize(NIXIE_NUM_TUBES,
                     ANODE0_PIN, ANODE1_PIN, ANODE2_PIN, ANODE3_PIN,
                     BCD0_PIN, BCD1_PIN, BCD2_PIN, BCD3_PIN, COMMA_PIN, &G.timeDigits);

    // initialize the timekeeper
    //Timekeeper.initialize();
    Time t = rtc.time();
    //if (rtc.lostPower()) {
    //   rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  // ToDo: automatically go into set time mode
    //}

    //if (!rtc.begin()) {
    //    // ToDo: display message on tubes to let user know that the RTC is not responding
    //    Serial.println("Couldn't find RTC");
    //    while (1)
    //        ;
    //}

    // enable the watchdog
    wdt_enable(WDT_TIMEOUT);
}

void loop() {
    // get current time from RTC
    Time now = rtc.time();  // ToDo: implement it into the chrono class
    int hours = now.hr;
    int minutes = now.min;
    int seconds = now.sec;

    getButtonStates();

    Nixie.refresh();  // refresh method is called many times across the code to ensure smooth display operation

    setDisplay();  // navigate the settings menu

    Nixie.refresh();
}

void getButtonStates(void) {
    // push button

    // tilt switch 0

    // tilt switch 1
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
    switch (G.menuState) {
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
    }
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
    G.timeDigits.value[0] = dec2bcdLow(G.systemTm->tm_min);
    G.timeDigits.value[1] = dec2bcdHigh(G.systemTm->tm_min);
    G.timeDigits.value[2] = dec2bcdLow(G.systemTm->tm_hour);
    G.timeDigits.value[3] = dec2bcdHigh(G.systemTm->tm_hour);
    /*G.dateDigits.value[0] = dec2bcdLow  (G.systemTm->tm_year);
  G.dateDigits.value[1] = dec2bcdHigh (G.systemTm->tm_year);
  G.dateDigits.value[2] = dec2bcdLow  (G.systemTm->tm_mon + 1);
  G.dateDigits.value[3] = dec2bcdHigh (G.systemTm->tm_mon + 1);
  G.dateDigits.value[4] = dec2bcdLow  (G.systemTm->tm_mday);
  G.dateDigits.value[5] = dec2bcdHigh (G.systemTm->tm_mday); */

    if (G.menuState == SHOW_TIME) {
        // trigger Nixie digit "Slot Machine" effect
        if (G.systemTm->tm_min != lastMin && (G.timeDigits.value[2] == 0)) {
            Nixie.slotMachine();
        }
        // trigger the cathode poisoning prevention routine
        if (G.cppEffectEnabled && G.timeDigits.value[0] == 0) {
            Nixie.cathodePoisonPrevent();
        }
    }
    lastMin = G.systemTm->tm_min;
}