/*
 * Nixie Clock main code
 * 
 * January 2020
 */

#include <time.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include "Timer.h"
#include "TimerTwo.h"
#include "Dcf.h"
#include "Nixie.h"
#include "Adc.h"
#include "Helper.h"
#include "Features.h"

#include <Wire.h>
#include "RTClib.h"

// Variables
#define MULTIPLEX_DELAY 2;     //Multiplex delay time
#define ANTIPOISING_DELAY 500; //anti poising delay time
RTC_DS3231 rtc;

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

void setup()
{

    pinMode(buttonPin, INPUT_PULLUP);
    pinMode(buttonPin2, INPUT_PULLUP);

    Serial.begin(9600);

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