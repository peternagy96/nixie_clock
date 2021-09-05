/*
 * Nixie Clock main code
 *
 * January 2020
 */

#include <Arduino.h>
#include <Wire.h>
//#include <time.h>
#include "avr/wdt.h"
//#include <TimeLib.h>
#include <DS3232RTC.h>

#include "Alarm.h"
#include "Buzzer.h"
#include "Countdown.h"
#include "Nixie.h"
#include "PushButton.h"
#include "TiltSwitch.h"
#include "Timekeeper.h"

// Variables
#define ANTIPOISING_DELAY 500  // anti poising delay time
#define SERIAL_BAUD 9600       // serial baud rate
#define WDT_TIMEOUT WDTO_4S    // timeout length of the watchdog timer
#define SECOND 1000            // 1000

// Pin variables
#define ANODE0_PIN A0  //A4 1MIN
#define ANODE1_PIN A1  //A3 10MIN
#define ANODE2_PIN A2  //   COMMA
#define ANODE3_PIN A3  //A1 1HR
#define ANODE4_PIN 6  //A0 10HR

#define BCD0_PIN 5
#define BCD1_PIN 4
#define BCD2_PIN 3
#define BCD3_PIN 2

#define BUZZER_PIN 12

// button pins
#define BUTTON0_APIN A6        // push button 0 - "mode"
#define BUTTON1_DOWN_APIN 7  // tilt button 1 - "increase"
#define BUTTON1_UP_APIN 11    // tilt button 1 - "decrease"
#define BUTTON2_DOWN_APIN 10   // tilt button 2 - "increase"
#define BUTTON2_UP_APIN 9     // tilt button 2 - "decrease"

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
    SET_ALARM2_MIN,
    SHOW_CHESS,
    SET_CHESS_1,
    SET_CHESS_2,
    SHOW_POMODORO,
    SET_POMODORO_1_MIN,
    SET_POMODORO_1_SEC,
    SET_POMODORO_2_MIN,
    SET_POMODORO_2_SEC
};

/*
 * Global variables
 */
typedef struct {
    MenuState_e menuState = SHOW_TIME;  // stores the state in the menu state machine
    NixieDigits_s timeDigits;           // stores the Nixie display digit values of the current time
    uint32_t secTs = 0;
    bool secFlag = true;
    bool twoSecFlag = true;
    bool quarterSecFlag = true;
    bool showAlarmFlag = false;
    bool rtcSynced = false;
    bool restMode = false;
    bool alarmWasOff = true;
    bool specialMode = false;
    bool chessWasNotSet = true;
    bool pomodoroWasNotSet = true;
    int activePomodoro = 1;
    uint32_t pomodoroTs = 0;
    int tubeOff = 0;
    uint8_t secCnt = 0;
    int timeOffset = 0;
} G_t;

G_t G;

// create objects
PushButtonClass PushButton;
TiltSwitchClass TiltSwitch[2];
AlarmClass Alarm[2];
CountdownClass Countdown[5]; // 0: countdown, 1: chess white, 2: chess black, 3: pomodoro 1, 4: pomodoro 2

void setup() {
    wdt_disable();  // disable watchdog

    setSyncProvider(RTC.get);

    // tmElements_t tm;
    // int year_init = CalendarYrToTm(2020);
    // tm.Year = year_init;
    // tm.Month = 1;
    // tm.Day = 1;
    // tm.Hour = 12;
    // tm.Minute = 0;
    // tm.Second = 0;

    // time_t systemTm;
    // systemTm = makeTime(tm);
    // RTC.set(systemTm);  // use the time_t value to ensure correct weekday is set
    // setTime(systemTm);

    //Serial.begin(SERIAL_BAUD); // ! Comment out if not debugging

    if (RTC.get() != 0) {
        G.rtcSynced = true;
        Serial.println("Successfully connected to the RTC!");
    } else {
        Serial.println("Could not connect to the RTC!");
    }


    Nixie.initialize(ANODE0_PIN, ANODE1_PIN, ANODE2_PIN, ANODE3_PIN, ANODE4_PIN,
                     BCD0_PIN, BCD1_PIN, BCD2_PIN, BCD3_PIN, &G.timeDigits);

    Timekeeper.initialize();
    Alarm[0].initialize();
    Alarm[1].initialize();
    Countdown[0].initialize(4, 0);
    Countdown[1].initialize(15, 0);
    Countdown[2].initialize(15, 0);
    Countdown[3].initialize(0, 30);
    Countdown[4].initialize(1, 0);
    Buzzer.initialize(BUZZER_PIN);

    PushButton.setPin(BUTTON0_APIN);
    TiltSwitch[0].setPin(BUTTON1_UP_APIN, BUTTON1_DOWN_APIN);
    TiltSwitch[1].setPin(BUTTON2_UP_APIN, BUTTON2_DOWN_APIN);

    wdt_enable(WDT_TIMEOUT);
}

void loop() {

    // if (G.quarterSecFlag) {
    //     G.quarterSecFlag = false;
    // }

    if (G.secFlag) {
        G.twoSecFlag = !G.twoSecFlag;
        G.secFlag = false;
        G.secTs = millis();
        if (G.secCnt < 3 && G.showAlarmFlag) {
            G.secCnt++;
        } else if (G.showAlarmFlag) {
            G.secCnt = 0;
            G.showAlarmFlag = false;
        }

        if (G.rtcSynced == false && timeStatus() == timeSet) {
            setSyncProvider(RTC.get);
            if (RTC.get() != 0) {
                G.rtcSynced = true;
                Serial.println("Successfully connected to the RTC!");
            } else {
                Serial.println("Could not connect to the RTC!");
            }
        }

        if (timeStatus() != timeSet) {
            G.rtcSynced = false;
        }

        Timekeeper.incrementSec();
        Countdown[0].loopHandler();
        Countdown[1].loopHandler();
        Countdown[2].loopHandler();
        Countdown[3].loopHandler();
        Countdown[4].loopHandler();
    }

    Nixie.refresh();  // refresh method is called many times across the code to ensure smooth display operation

    if (minute() == 0 and second() == 0) {
        // this code runs once an hour
        Nixie.cppEnabled = true;
    }

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

    // if (millis() - G.secTs >= SECOND/4) {
    //     G.quarterSecFlag = true;
    // }

    displayMenu();

    Nixie.refresh();

    //specialmodeLoophandler();

    Nixie.refresh();
}

void getButtonStates(void) {
    if (G.restMode == false) {
        PushButton.readState();
    }
    TiltSwitch[0].readState();
    TiltSwitch[1].readState();
}

void alarmLoophandler(void) {
    if (TiltSwitch[0].up && !Alarm[0].isBeingSet) {
        Alarm[0].alarmGoesOff();
    } else if (TiltSwitch[0].down && !Alarm[1].isBeingSet) {
        Alarm[1].alarmGoesOff();
    }
    Alarm[0].autoTurnoff();
    Alarm[1].autoTurnoff();
    Countdown[0].autoTurnoff();
    Countdown[1].autoTurnoff();
    Countdown[2].autoTurnoff();
    if (Alarm[0].alarm || Alarm[1].alarm || Countdown[0].alarm
        || Countdown[1].alarm || Countdown[2].alarm
        || Countdown[3].alarm || Countdown[4].alarm ) {
        Buzzer.playMelody1();
    } else {
        Buzzer.stop();
    }

    if (TiltSwitch[0].middle) {
        Alarm[0].alarm = false;
        Alarm[1].alarm = false;
        Alarm[0].active = true;
        Alarm[1].active = true;
    }
}

void specialmodeLoophandler(void) {
    if (PushButton.reallyLongPress()) {
        G.menuState = SHOW_TIME;
        G.specialMode = !G.specialMode;
        if (G.specialMode == false) {
            Nixie.enable(true);
            Buzzer.stop();
            adjustTime(-G.timeOffset);
            G.timeOffset = 0;
        }
    }
    if (G.specialMode) {
        if (year() == 2020 && month() == 10 && day() > 9) {
            if (hour() == 3 && minute() == 0 && second() == 0 && G.secFlag) {
                adjustTime(-G.timeOffset);
                G.timeOffset = random(-20, 20);
                adjustTime(G.timeOffset);
                Timekeeper.initialize();
            }

            if (day() > 16 && hour() == 23 && minute() == 12 && second() < 21) {
                    Buzzer.playMelody1();
            }
            if (day() > 23) {
                if (hour() == 3 && minute() == 0 && second() == 0 && G.secFlag) {
                    Nixie.enablePin((uint8_t) G.tubeOff);
                    G.tubeOff = random(0, 5);
                    Nixie.disablePin((uint8_t) G.tubeOff);
                }
                if (hour() > 20 || hour() < 4) {
                    Nixie.digits->blank[(uint8_t) G.tubeOff];
                }
            }
        }
    }
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
            Nixie.refresh();
            if (TiltSwitch[1].up && (hour() < 6)) {
                Nixie.enable(false);
                G.restMode = true;
            } else if (TiltSwitch[1].down) {
                Nixie.enable(false);
                G.restMode = true;
            } else {
                if (G.restMode == true) {
                    Nixie.slotMachine();
                    Nixie.enable(true);
                }
                G.restMode = false;
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
            Countdown[0].displayTime(G.timeDigits);
            Nixie.blinkNone();
            if (TiltSwitch[1].up) {
                Countdown[0].stopwatch();
            } else if (TiltSwitch[1].down) {
                if (Countdown[0].enabled) {
                    Countdown[0].start();
                }
            } else {
                Countdown[0].reset();
                Buzzer.stop();
            }

            break;
        case SET_TIMER_MIN:
            Countdown[0].displayTime(G.timeDigits);
            if (TiltSwitch[1].up) {
                Nixie.blinkNone();
                Countdown[0].setTimeSlow("min", "+");
            } else if (TiltSwitch[1].down) {
                Nixie.blinkNone();
                Countdown[0].setTimeSlow("min", "-");
            } else {
                Nixie.blinkLeft();
            }
            break;
        case SET_TIMER_SEC:
            Countdown[0].displayTime(G.timeDigits);
            if (TiltSwitch[1].up) {
                Nixie.blinkNone();
                Countdown[0].setTimeSlow("sec", "+");
            } else if (TiltSwitch[1].down) {
                Nixie.blinkNone();
                Countdown[0].setTimeSlow("sec", "-");
            } else {
                Nixie.blinkRight();
            }
            break;
        case SHOW_DATE:
            Timekeeper.displayDate(G.timeDigits);
            Timekeeper.isBeingSet = false;
            Nixie.blinkNone();
            if (G.secFlag) {
                G.menuState = SHOW_YEAR;
            }
            break;
        case SHOW_YEAR:
            Timekeeper.displayYear(G.timeDigits);
            Timekeeper.isBeingSet = false;
            Nixie.blinkNone();
            if (G.twoSecFlag) {
                G.menuState = SHOW_DATE;
            }
            break;
        case SET_MONTH:
            Timekeeper.displayDate(G.timeDigits);
            Timekeeper.isBeingSet = true;
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
            Timekeeper.isBeingSet = true;
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
            Timekeeper.isBeingSet = true;
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
            Nixie.blinkNone();
            Alarm[0].active = true;
            if (Alarm[0].isBeingSet) {
                G.showAlarmFlag = true;
                Alarm[0].isBeingSet = false;
            }
            Alarm[0].displayTime(G.timeDigits);
            if (!G.showAlarmFlag) {
                G.menuState = SHOW_TIME;
            }
            break;
        case SET_ALARM1_HOUR:
            Alarm[0].isBeingSet = true;
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
            Alarm[0].isBeingSet = true;
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
            Nixie.blinkNone();
            Alarm[1].active = true;
            if (Alarm[1].isBeingSet) {
                G.showAlarmFlag = true;
                Alarm[1].isBeingSet = false;
            }
            Alarm[1].displayTime(G.timeDigits);
            if (!G.showAlarmFlag) {
                G.menuState = SHOW_TIME;
            }
            break;
        case SET_ALARM2_HOUR:
            Alarm[1].isBeingSet = true;
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
            Alarm[1].isBeingSet = true;
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
        case SHOW_CHESS:

            Nixie.blinkNone();
            G.chessWasNotSet = true;
            if (TiltSwitch[1].up) {
                Countdown[2].stop();
                Countdown[1].displayTime(G.timeDigits);
                if (Countdown[1].enabled) {
                    Countdown[1].start();
                }
            } else if (TiltSwitch[1].down) {
                Countdown[1].stop();
                Countdown[2].displayTime(G.timeDigits);
                if (Countdown[2].enabled) {
                    Countdown[2].start();
                }
            } else {
                if (G.twoSecFlag == true) {
                    Countdown[1].displayTime(G.timeDigits);
                }
                else {
                    Countdown[2].displayTime(G.timeDigits);
                }
                Countdown[1].stop();
                Countdown[2].stop();
                Buzzer.stop();
            }
            break;
        case SET_CHESS_1:
            if (G.chessWasNotSet) {
                Countdown[1].resetTime();
                Countdown[2].resetTime();
                G.chessWasNotSet = false;
            }

            Countdown[1].displayTime(G.timeDigits);
            if (TiltSwitch[1].up) {
                Nixie.blinkNone();
                Countdown[1].setTimeSlow("min", "+");
            } else if (TiltSwitch[1].down) {
                Nixie.blinkNone();
                Countdown[1].setTimeSlow("min", "-");
            } else {
                Nixie.blinkAll();
            }
            Countdown[1].setDefault();
            break;
        case SET_CHESS_2:

            Countdown[2].displayTime(G.timeDigits);
            if (TiltSwitch[1].up) {
                Nixie.blinkNone();
                Countdown[2].setTimeSlow("min", "+");
            } else if (TiltSwitch[1].down) {
                Nixie.blinkNone();
                Countdown[2].setTimeSlow("min", "-");
            } else {
                Nixie.blinkAll();
            }
            Countdown[2].setDefault();
            break;
        case SHOW_POMODORO:
            Nixie.blinkNone();
            G.pomodoroWasNotSet = true;

            // reset pomodoro timer to last set value
            if (TiltSwitch[1].up) {
                Countdown[3].resetTime();
                Countdown[4].resetTime();
                if (G.twoSecFlag == true) {
                    Countdown[3].displayTime(G.timeDigits);
                }
                else {
                    Countdown[4].displayTime(G.timeDigits);
                }

            // run pomodoro timer infinitely
            } else if (TiltSwitch[1].down) {
                if (G.activePomodoro == 1) {
                    if (Countdown[4].alarm & ((millis() - G.pomodoroTs) > 1000)) {
                        Countdown[4].reset();
                    }
                    Countdown[3].displayTime(G.timeDigits);
                    if (Countdown[3].enabled) {
                        Countdown[3].restart();
                    }
                    if (Countdown[3].alarm) {
                        G.activePomodoro = 2;
                        G.pomodoroTs = millis();
                    }
                } else if (G.activePomodoro == 2) {
                    if (Countdown[3].alarm & ((millis() - G.pomodoroTs) > 1000)) {
                        Countdown[3].reset();

                    }
                    Countdown[4].displayTime(G.timeDigits);
                    if (Countdown[4].enabled) {
                        Countdown[4].restart();
                    }
                    if (Countdown[4].alarm) {
                        G.activePomodoro = 1;
                        G.pomodoroTs = millis();
                    }
                }


            } else {
                if (G.twoSecFlag == true) {
                    Countdown[3].displayTime(G.timeDigits);
                }
                else {
                    Countdown[4].displayTime(G.timeDigits);
                }
                Countdown[3].stop();
                Countdown[4].stop();
                Buzzer.stop();
            }
            break;
        case SET_POMODORO_1_MIN:
            if (G.pomodoroWasNotSet) {
                Countdown[3].resetAlarm();
                Countdown[4].resetAlarm();
                Countdown[3].resetTime();
                Countdown[4].resetTime();
                G.pomodoroWasNotSet = false;
            }

            Countdown[3].displayTime(G.timeDigits);
            if (TiltSwitch[1].up) {
                Nixie.blinkNone();
                Countdown[3].setTimeSlow("min", "+");
            } else if (TiltSwitch[1].down) {
                Nixie.blinkNone();
                Countdown[3].setTimeSlow("min", "-");
            } else {
                Nixie.blinkLeft();
            }
            Countdown[3].setDefault();
            break;
        case SET_POMODORO_1_SEC:
            Countdown[3].displayTime(G.timeDigits);
            if (TiltSwitch[1].up) {
                Nixie.blinkNone();
                Countdown[3].setTimeSlow("sec", "+");
            } else if (TiltSwitch[1].down) {
                Nixie.blinkNone();
                Countdown[3].setTimeSlow("sec", "-");
            } else {
                Nixie.blinkRight();
            }
            Countdown[3].setDefault();
            break;
        case SET_POMODORO_2_MIN:
            Countdown[4].displayTime(G.timeDigits);
            if (TiltSwitch[1].up) {
                Nixie.blinkNone();
                Countdown[4].setTimeSlow("min", "+");
            } else if (TiltSwitch[1].down) {
                Nixie.blinkNone();
                Countdown[4].setTimeSlow("min", "-");
            } else {
                Nixie.blinkLeft();
            }
            Countdown[4].setDefault();
            break;
        case SET_POMODORO_2_SEC:
            Countdown[4].displayTime(G.timeDigits);
            if (TiltSwitch[1].up) {
                Nixie.blinkNone();
                Countdown[4].setTimeSlow("sec", "+");
            } else if (TiltSwitch[1].down) {
                Nixie.blinkNone();
                Countdown[4].setTimeSlow("sec", "-");
            } else {
                Nixie.blinkRight();
            }
            Countdown[4].setDefault();
            break;
    }
}

void navigateMenu(void) {
    // -> set next state depending on button states

    switch (G.menuState) {
        case SHOW_TIME:
            if (PushButton.falling() && TiltSwitch[1].middle) {
                G.menuState = SHOW_DATE;
            } else if (PushButton.longPress() && TiltSwitch[1].middle) {
                if (TiltSwitch[0].middle) {
                    G.menuState = SET_HOUR;
                } else if (TiltSwitch[0].up) {
                    G.menuState = SET_ALARM1_HOUR;
                } else if (TiltSwitch[0].down) {
                    G.menuState = SET_ALARM2_HOUR;
                }
            }
            //Nixie.refresh();
            if (G.alarmWasOff) {
                if (TiltSwitch[0].up) {
                    G.menuState = SHOW_ALARM1;
                    G.alarmWasOff = false;
                    G.showAlarmFlag = true;
                } else if (TiltSwitch[0].down) {
                    G.menuState = SHOW_ALARM2;
                    G.alarmWasOff = false;
                    G.showAlarmFlag = true;
                }
            }
            //Nixie.refresh();
            if (TiltSwitch[0].middle) {
                    G.alarmWasOff = true;
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
                G.menuState = SHOW_CHESS;
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
            G.alarmWasOff = false;
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
            G.alarmWasOff = false;
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
        case SHOW_CHESS:
            if (PushButton.falling()) {
                G.menuState = SHOW_POMODORO;
            } else if (PushButton.longPress()) {
                G.menuState = SET_CHESS_1;
            }
            break;
        case SET_CHESS_1:
            if (PushButton.falling()) {
                G.menuState = SET_CHESS_2;
            }
            break;
        case SET_CHESS_2:
            if (PushButton.falling()) {
                G.menuState = SHOW_CHESS;
            }
            break;
        case SHOW_POMODORO:
            if (PushButton.falling() && TiltSwitch[1].middle) {
                G.menuState = SHOW_TIME;
            } else if (PushButton.longPress()) {
                G.menuState = SET_POMODORO_1_MIN;
            }
            break;
        case SET_POMODORO_1_MIN:
            if (PushButton.falling() && TiltSwitch[1].middle) {
                    G.menuState = SET_POMODORO_1_SEC;
                }
            break;
        case SET_POMODORO_1_SEC:
            if (PushButton.falling() && TiltSwitch[1].middle) {
                    G.menuState = SET_POMODORO_2_MIN;
                }
            break;
        case SET_POMODORO_2_MIN:
            if (PushButton.falling() && TiltSwitch[1].middle) {
                    G.menuState = SET_POMODORO_2_SEC;
                }
            break;
        case SET_POMODORO_2_SEC:
            if (PushButton.falling() && TiltSwitch[1].middle) {
                    G.menuState = SHOW_POMODORO;
                }
            break;
    }
}
