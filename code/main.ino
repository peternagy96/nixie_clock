/*
 * Nixie Clock main code
 * 
 * January 2020
 */

#include "Buzzer.h"
#include "Helper.h"
#include "Nixie.h"
#include "PushButton.h"
#include "TiltSwitch.h"
#include "Timekeeper.h"
#include "Timer.h"

#include <Wire.h>
#include "RTClib/DS1302.h"

// Variables
#define MULTIPLEX_DELAY 2;     //Multiplex delay time
#define ANTIPOISING_DELAY 500; //anti poising delay time

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
#define BUTTON0_APIN A2       // push button 0 - "mode"
#define BUTTON1_UP_APIN A3    // tilt button 1 - "increase"
#define BUTTON1__DOWN_APIN A1 // tilt button 1 - "decrease"
#define BUTTON2_UP_APIN A3    // tilt button 2 - "increase"
#define BUTTON2_DOWN_APIN A1  // tilt button 2 - "decrease"

// RTC pins
#define RTC_EnablePin 5
#define RTC_IOPin 6
#define RTC_SerialPin 7
DS1302 rtc(RTC_EnablePin, RTC_IOPin, RTC_SerialPin);

void setup()
{

    uint8_t i;
    time_t sysTime;
    MCUSR = 0;     // clear MCU status register
    wdt_disable(); // and disable watchdog

    #ifndef SERIAL_DEBUG
    Serial.begin(9600)
    #endif

        PRINTLN(" ");
    PRINTLN("+ + +  N I X I E  C L O C K  + + +");
    PRINTLN(" ");

    // initialize the Nixie tube display
    Nixie.initialize(NIXIE_MAX_NUM_TUBES,
                     ANODE0_PIN, ANODE1_PIN, ANODE2_PIN, ANODE3_PIN, ANODE4_PIN, ANODE5_PIN,
                     BCD0_PIN, BCD1_PIN, BCD2_PIN, BCD3_PIN, COMMA_PIN, &G.timeDigits);

    // reset system time
    set_system_time(0);
    sysTime = time(NULL);
    G.systemTm = localtime(&sysTime);

    // derive the seconds-per-day correction value from the current timer1Period
    Settings.secPerDayCorrect = TIMER1_TO_SEC_PER_DAY(Settings.timer1Period);

    // initialize Timer 1 to trigger timer1ISR once per second
    Timekeeper.initialize(Settings.timekeeperPeriod);
    Timekeeper.attachInterrupt(timekeeperISR);

    // initialize Timer2, set the period to 25ms (1s / 40)
    // Timer2 is used for Chronometer and Countdown Timer features
    // Timer2 has a maximum period of 32768us
    Countdown.initialize(Settings.timer1Period / 40);
    Countdown.attachInterrupt(timer2ISR);
    cli();
    Countdown.stop();
    Countdown.restart();
    sei();
    G.countdownSecCounter = 0;
    G.countdownTenthCounter = 0;

    #ifndef SERIAL_DEBUG
    // initialize the Buzzer driver (requires serial communication pin)
    Buzzer.initialize(BUZZER_PIN);
    #endif

    // initialize the alarm, countdown timer and stopwatch
    Alarm.initialize(&Settings.alarm);
    CdTimer.initialize(featureCallback);
    Countdown.initialize(featureCallback);

    // enable the watchdog
    wdt_enable(WDT_TIMEOUT);

    //pinMode(buttonPin, INPUT_PULLUP);
    //pinMode(buttonPin2, INPUT_PULLUP);

    if (rtc.lostPower())
    {
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // ToDo: automatically go into set time mode
    }
    //delay(3000); // wait for console opening
    if (!rtc.begin())
    {
        // ToDo: display message on tubes to let user know that the RTC is not responding
        Serial.println("Couldn't find RTC");
        while (1)
            ;
    }

    pinMode(13, OUTPUT); //LED on Arduino

    pinMode(12, OUTPUT); // 74141 A
    pinMode(11, OUTPUT); // 74141 B
    pinMode(10, OUTPUT); // 74141 C
    pinMode(9, OUTPUT);  // 74141 D

    pinMode(8, OUTPUT); //anode Nixie1
    pinMode(7, OUTPUT); //anode Nixie2
    pinMode(6, OUTPUT); //anode Nixie3
    pinMode(5, OUTPUT); //anode Nixie4

    pinMode(4, OUTPUT); //seconds

    digitalWrite(13, 1); //LED on Arduino ON

    digitalWrite(9, 0);
    digitalWrite(10, 0);
    digitalWrite(11, 0);
    digitalWrite(12, 0);

    digitalWrite(8, 0); //anode
    digitalWrite(7, 0); //anode
    digitalWrite(6, 0); //anode
    digitalWrite(5, 0); //anode

    digitalWrite(4, 1); //seconds
}

void loop()
{

    // read button states - with debouncing included

    // go to display corresponding to the states of the buttons

    // read and update time from RTC
}