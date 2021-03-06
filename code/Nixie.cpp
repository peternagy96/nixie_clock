/*
* Nixie tube display class
*/

#include "Nixie.h"
#include <avr/wdt.h>
#include "Arduino.h"

#define DIGIT_PERIOD 3200
#define MAX_ON_DURATION 2680 //2680
#define BLINK_PERIOD 500000
#define SLOT_MACHINE_PERIOD 40000
#define CPP_PERIOD 200000
#define MULTIPLEX_DELAY 2  // multiplex delay time

NixieClass Nixie;

void NixieClass::initialize(uint8_t anodePin0, uint8_t anodePin1, uint8_t anodePin2, uint8_t anodePin3,
                            uint8_t anodePin4, uint8_t bcdPin0, uint8_t bcdPin1, uint8_t bcdPin2,
                            uint8_t bcdPin3, NixieDigits_s *digits) {
    uint8_t i;
    this->numTubes = 5;
    this->anodePin[0] = anodePin0;
    this->anodePin[1] = anodePin1;
    this->anodePin[2] = anodePin2;
    this->anodePin[3] = anodePin3;
    this->anodePin[4] = anodePin4;
    this->bcdPin[0] = bcdPin0;
    this->bcdPin[1] = bcdPin1;
    this->bcdPin[2] = bcdPin2;
    this->bcdPin[3] = bcdPin3;
    this->digits = digits;
    this->digitOnDuration = MAX_ON_DURATION;  // * map(brightness, 0, 255, 0, MAX_ON_DURATION) -> possibility to set brightness
    this->digitPeriod = DIGIT_PERIOD;

    // initialize output pins
    for (i = 0; i < numTubes; i++) {
        pinMode(anodePin[i], OUTPUT);
        digitalWrite(anodePin[i], LOW);
    }
    for (i = 0; i < 4; i++) {
        pinMode(bcdPin[i], OUTPUT);
        digitalWrite(bcdPin[i], LOW);
    }
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

        if (digit != 2) {
            bcdVal = digits->value[digit];
            if (slotMachineEnabled[digit] || cppEnabled) {
                // produce "Slot Machine" or CPP effect
                bcdVal += slotMachineCnt[digit] + cppCnt;
                while (bcdVal > 9)
                    bcdVal -= 10;
            }
            anodeVal = !(blinkFlag && digits->blnk[digit]) && !digits->blank[digit];
            digitalWrite(bcdPin[0], bcdVal & 1);
            digitalWrite(bcdPin[1], (bcdVal >> 1) & 1);
            digitalWrite(bcdPin[2], (bcdVal >> 2) & 1);
            digitalWrite(bcdPin[3], (bcdVal >> 3) & 1);
            digitalWrite(anodePin[digit], anodeVal);
        } else {
            commaVal = !(blinkFlag && digits->blnk[digit]) && !digits->blank[digit];
            digitalWrite(anodePin[digit], commaVal);
        }

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
    if (digit != 2 && slotMachineEnabled[digit]) {
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

void NixieClass::blinkNone(void) {
    digits->blnk[0] = false;
    digits->blnk[1] = false;
    digits->blnk[3] = false;
    digits->blnk[4] = false;
}

void NixieClass::blinkAll(void) {
    digits->blnk[0] = true;
    digits->blnk[1] = true;
    digits->blnk[3] = true;
    digits->blnk[4] = true;
}

void NixieClass::blinkLeft(void) {
    digits->blnk[0] = false;
    digits->blnk[1] = false;
    digits->blnk[3] = true;
    digits->blnk[4] = true;
}

void NixieClass::blinkRight(void) {
    digits->blnk[0] = true;
    digits->blnk[1] = true;
    digits->blnk[3] = false;
    digits->blnk[4] = false;
}

void NixieClass::resetBlinking(void) {
    blinkTs = micros();
    blinkFlag = false;
}

void NixieClass::slotMachine(void) {
    uint8_t i;
    for (i = 0; i < numTubes; i++) {
        if (i != 2) {
            slotMachineEnabled[i] = true;
            slotMachineCnt[i] = slotMachineCntStart[i];
            slotMachineDelay[i] = 0;
        }
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
}

void NixieClass::disablePin(uint8_t pin) {
    digitalWrite(bcdPin[pin], LOW);
}

void NixieClass::enablePin(uint8_t pin) {
    digitalWrite(bcdPin[pin], HIGH);
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
        output->blnk[i] = false;
    }
    output->comma_blnk = 0;
    output->numDigits = NIXIE_NUM_TUBES - 1;
}