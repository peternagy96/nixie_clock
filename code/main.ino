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
#define SECOND 1000

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

// analog pins
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
    uint32_t secTs = -1;
} G_t;

G_t G;
//Time systemTm = rtc.time();
Time systemTm(2020, 1, 1, 0, 0, 0, Time::kMonday);

// create objects
PushButtonClass PushButton;
TiltSwitchClass TiltSwitch[2];
AlarmClass Alarm;
TimerClass Timer;

void setup() {
    wdt_disable();  // and disable watchdog

    Serial.begin(SERIAL_BAUD);

    Nixie.initialize(ANODE0_PIN, ANODE1_PIN, ANODE2_PIN, ANODE3_PIN, ANODE4_PIN,
                     BCD0_PIN, BCD1_PIN, BCD2_PIN, BCD3_PIN, &G.timeDigits);

    Timekeeper.initialize(&systemTm);
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
        G.menuState = SET_HOUR;
    }

    // enable the watchdog
    wdt_enable(WDT_TIMEOUT);
}

void loop() {
    //systemTm = defaultTm;  //rtc.time();  // ToDo: implement it into the chrono class

    if ((G.secTs >= SECOND) || (G.secTs == -1)) {
        G.secTs = millis();
        Timekeeper.incrementSec();
    }

    if (G.menuState == SET_YEAR) {
        Serial.println("menu state is set date");
    }

    Nixie.refresh();  // refresh method is called many times across the code to ensure smooth display operation

    getButtonStates();

    Nixie.refresh();  // refresh method is called many times across the code to ensure smooth display operation

    Buzzer.readState();

    Nixie.refresh();

    //navigateMenu();

    Nixie.refresh();

    displayMenu();

    Nixie.refresh();
}

void getButtonStates(void) {
    PushButton.readState();
    TiltSwitch[0].readState();
    TiltSwitch[1].readState();
}

void displayMenu(void) {
    // show the proper digits and setup proper blinking
    // depending on the tilt switch states increment/decrement variables
    switch (G.menuState) {
        case SHOW_TIME:
            Timekeeper.displayTime(G.timeDigits);
            Nixie.blinkNone();
            basicTiltMode();
        case SET_HOUR:
            Timekeeper.displayTime(G.timeDigits);
            Nixie.blinkLeft();
            if (TiltSwitch[1].up) {
                // ! increase hours every 1/4 secs
            } else if (TiltSwitch[1].down) {
                // ! decrease hours every 1/4 secs
            }
        case SET_MIN:
            Timekeeper.displayTime(G.timeDigits);
            Nixie.blinkRight();
            if (TiltSwitch[1].up) {
                // ! increase hours every 1/4 secs
            } else if (TiltSwitch[1].down) {
                // ! decrease hours every 1/4 secs
            }
        case SHOW_TIMER:
            if (TiltSwitch[1].up || TiltSwitch[1].down) {
                // ! enable timer
            }
        case SET_TIMER_MIN:
            if (TiltSwitch[1].up) {
                // ! increase timer mins
            } else if (TiltSwitch[1].down) {
                // ! decrease timer mins
            }
        case SET_TIMER_SEC:
            // ! blink seconds tubes
            if (TiltSwitch[1].up) {
                // ! increase timer secs
            } else if (TiltSwitch[1].down) {
                // ! decrease timer secs
            }
        case SHOW_DATE:
            Timekeeper.displayDate(G.timeDigits);
            Nixie.blinkNone();
            basicTiltMode();
            // ! switch to show_year and back every second
        case SHOW_YEAR:
            Timekeeper.displayYear(G.timeDigits);
            Nixie.blinkNone();
            basicTiltMode();
            // ! switch to show_year and back every second
        case SET_MONTH:
            Timekeeper.displayDate(G.timeDigits);
            Nixie.blinkLeft();
        case SET_DAY:
            Timekeeper.displayDate(G.timeDigits);
            Nixie.blinkRight();
        case SET_YEAR:
            Timekeeper.displayYear(G.timeDigits);
            Nixie.blinkAll();
        case SHOW_ALARM1:
            // ! show alarm time for a second when toggle switch is set, then switch to show time
            break;
        case SET_ALARM1_HOUR:
            break;
        case SET_ALARM1_MIN:
            break;
        case SHOW_ALARM2:
            break;
        // ! show alarm time for a second when toggle switch is set, then switch to show time
        case SET_ALARM2_HOUR:
            break;
        case SET_ALARM2_MIN:
            break;
    }
}

void navigateMenu(void) {
    // -> set next state depending on button states
    if (PushButton.rising()) {
        switch (G.menuState) {
            case SHOW_TIME:
                Nixie.blinkNone();
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
                Nixie.blinkLeft();
                if (PushButton.falling()) {
                    G.menuState = SET_MIN;
                }
            case SET_MIN:
                Nixie.blinkRight();
                if (PushButton.falling()) {
                    G.menuState = SHOW_TIME;
                }
            case SHOW_TIMER:
                if (PushButton.falling()) {
                    G.menuState = SHOW_TIME;
                } else if (PushButton.longPress()) {
                    G.menuState = SET_TIMER_MIN;
                }
            case SET_TIMER_MIN:
                if (PushButton.falling()) {
                    G.menuState = SET_TIMER_SEC;
                }
            case SET_TIMER_SEC:
                if (PushButton.falling()) {
                    G.menuState = SHOW_TIMER;
                }
            case SHOW_DATE:
                Nixie.blinkNone();
                if (PushButton.falling()) {
                    G.menuState = SHOW_TIME;
                } else if (PushButton.longPress()) {
                    G.menuState = SET_MONTH;
                }
            case SHOW_YEAR:
                Nixie.blinkNone();
                if (PushButton.falling()) {
                    G.menuState = SHOW_TIME;
                } else if (PushButton.longPress()) {
                    G.menuState = SET_MONTH;
                }
            case SET_MONTH:
                Nixie.blinkLeft();
                // ! blink the month tubes
                if (PushButton.falling()) {
                    G.menuState = SET_DAY;
                }
            case SET_DAY:
                Nixie.blinkRight();
                if (PushButton.falling()) {
                    G.menuState = SET_YEAR;
                }
            case SET_YEAR:
                Nixie.blinkAll();
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