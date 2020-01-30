/*
* Nixie tube display class
*/

#include "Nixie.h"
#include <avr/wdt.h>
#include "Arduino.h"

#define DIGIT_PERIOD 3000
#define MAX_ON_DURATION 2680
#define BLINK_PERIOD 500000
#define SLOT_MACHINE_PERIOD 40000
#define CPP_PERIOD 200000

NixieClass Nixie;

void NixieClass::initialize(uint8_t anodePin0, uint8_t anodePin1, uint8_t anodePin2, uint8_t anodePin3,
                            uint8_t bcdPin0, uint8_t bcdPin1, uint8_t bcdPin2, uint8_t bcdPin3,
                            uint8_t commaPin, NixieDigits_s *digits) {
    uint8_t i;
    this->numTubes = 4;
    this->anodePin[0] = anodePin0;
    this->anodePin[1] = anodePin1;
    this->anodePin[2] = anodePin2;
    this->anodePin[3] = anodePin3;
    this->bcdPin[0] = bcdPin0;
    this->bcdPin[1] = bcdPin1;
    this->bcdPin[2] = bcdPin2;
    this->bcdPin[3] = bcdPin3;
    this->commaPin = commaPin;
    this->digits = digits;
    this->digitOnDuration = MAX_ON_DURATION;  // * map(brightness, 0, 255, 0, MAX_ON_DURATION) -> possibility to set brightness

    // initialize output pins
    for (i = 0; i < numTubes; i++) {
        pinMode(anodePin[i], OUTPUT);
        digitalWrite(anodePin[i], LOW);
    }
    for (i = 0; i < 4; i++) {
        pinMode(bcdPin[i], OUTPUT);
        digitalWrite(bcdPin[i], LOW);
    }
    pinMode(commaPin, OUTPUT);
    digitalWrite(commaPin, LOW);
}

void NixieClass::setDigits(NixieDigits_s *digits) {
    this->digits = digits;
}

void NixieClass::refresh(void)  // ToDo: rewrite and test this function
{
    uint8_t bcdVal, commaVal;
    bool anodeVal;
    uint32_t ts = micros();

    // display disabled
    if (!enabled) {
        wdt_reset();
        // keep rotating digits to avoid delayed "Slot Machine" effect
        digit++;
        if (digit >= numTubes)
            digit = 0;
    }

    // display multiplexing by switching digits
    else if (ts - lastTs >= DIGIT_PERIOD) {
        wdt_reset();  // reset the watchdog timer

        digitalWrite(anodePin[digit], LOW);

        digit++;
        if (digit >= numTubes)
            digit = 0;

        bcdVal = digits->value[digit];

        if (slotMachineEnabled[digit] || cppEnabled) {
            // produce "Slot Machine" or CPP effect
            bcdVal += slotMachineCnt[digit] + cppCnt;
            while (bcdVal > 9)
                bcdVal -= 10;
        }

        anodeVal = !(blinkFlag && (digits->blnk[digit] || blinkAllEnabled)) && !digits->blank[digit];

        digitalWrite(bcdPin[0], bcdVal & 1);
        digitalWrite(bcdPin[1], (bcdVal >> 1) & 1);
        digitalWrite(bcdPin[2], (bcdVal >> 2) & 1);
        digitalWrite(bcdPin[3], (bcdVal >> 3) & 1);
        digitalWrite(commaPin, commaVal);
        digitalWrite(anodePin[digit], anodeVal);

        lastTs = ts;
    }

    // turn off all opto-coupler controlled pins ahead of time, this avoids ghost numbers
    // also control brightness by reducing anode on time
    // reduce on duration by dimFactor for decimal points without digits
    else if (ts - lastTs >= digitOnDuration) {
        digitalWrite(anodePin[digit], LOW);
    }

    // toggle blinking digits
    if (ts - blinkTs > BLINK_PERIOD) {
        blinkFlag = !blinkFlag;
        blinkTs = ts;
    }

    // generate the digit "Slot Machine" effect
    if (slotMachineEnabled[digit]) {
        if (ts - slotMachineTs[digit] > slotMachineDelay[digit]) {
            slotMachineCnt[digit]++;
            if (slotMachineCnt[digit] >= slotMachineCntMax[digit]) {
                slotMachineEnabled[digit] = false;
                slotMachineCnt[digit] = 0;
            }
            slotMachineDelay[digit] = SLOT_MACHINE_PERIOD + 10 * (uint32_t)slotMachineCnt[digit] * (uint32_t)slotMachineCnt[digit];
            slotMachineTs[digit] = ts;
        }
    }

    // generate cathode poisoning prevention effect
    if (cppEnabled) {
        if (ts - cppTs > CPP_PERIOD) {
            cppCnt++;
            if (cppCnt >= 20)
                cppEnabled = false, cppCnt = 0;
            cppTs = ts;
        }
    }
}

void NixieClass::blinkAll(bool enable) {
    blinkAllEnabled = enable;
}

void NixieClass::resetBlinking(void) {
    blinkTs = micros();
    blinkFlag = false;
}

void NixieClass::slotMachine(void) {
    uint8_t i;
    for (i = 0; i < numTubes; i++) {
        slotMachineEnabled[i] = true;
        slotMachineCnt[i] = slotMachineCntStart[i];
        slotMachineDelay[i] = 0;
    }
}

void NixieClass::cathodePoisonPrevent(void) {
    cppEnabled = true;
    cppCnt = 0;
    cppTs = micros() - CPP_PERIOD;
}

void NixieClass::blank(void) {
    uint8_t i;
    for (i = 0; i < numTubes; i++)
        digitalWrite(anodePin[i], LOW);
    for (i = 0; i < 4; i++)
        digitalWrite(bcdPin[i], LOW);
    digitalWrite(commaPin, LOW);
}

void NixieClass::enable(bool enable) {
    enabled = enable;
    if (!enabled)
        blank();
}

void NixieClass::dec2bcd(uint32_t value, NixieDigits_s *output, uint8_t numDigits) {
    uint8_t i, n;
    uint64_t p = 1;
    //bool leadingZero = true;

    n = numDigits;

    for (i = 0; i < n; i++)
        output->value[i] = 0;
    for (i = 0; i < n; i++)
        p = p * 10;

    while (value >= p)
        value -= p;

    for (i = 0; i < n; i++) {
        p = p / 10;
        while (value >= p) {
            value -= p;
            output->value[n - i - 1]++;
        }
        //if (output->value[i] > 0) leadingZero = false;
        //if (leadingZero) output->blank[i] = true;
        //else output->blank[i] = false;
    }
}

void NixieClass::resetDigits(NixieDigits_s *output) {
    int8_t i;

    for (i = 0; i < NIXIE_NUM_TUBES; i++) {
        output->value[i] = 0;
        output->blank[i] = false;
        output->comma[i] = false;
        output->blnk[i] = false;
    }
    output->numDigits = NIXIE_NUM_TUBES;
}