/*
* Nixie tube display class
*/

#ifndef __NIXIE_H
#define __NIXIE_H

#include <stdint.h>
#include "DS1302.h"

#define NIXIE_NUM_TUBES 5

/*
 * Array of Nixie tube digit values
 */
struct NixieDigits_s {
    uint8_t value[NIXIE_NUM_TUBES] = {0};                             /* BCD value */
    bool blank[NIXIE_NUM_TUBES] = {false, false, true, false, false}; /* blank digit */
    uint8_t comma_blnk = 1;                                           /* comma blinking pattern, 0 - no blinking, 1 - single, 2 - double */
    bool blnk[NIXIE_NUM_TUBES] = {true, true, true, true, true};      /* enable blinking */
    uint8_t numDigits = NIXIE_NUM_TUBES - 1;
};

/*
 * Main class
 */
class NixieClass {
   public:
    /*
     * Initialize the hardware
     * Parameters:
     *   anodePin0..5 : anode control pins
     *   bcdPin0..5   : pins connected to the BCD to decimal anode driver decoder chip (74141, K155ID1)
     *   commaPin     : pin connected to the decimal point symbol
     *   digits       : pointer to the nixie digits structure
     */
    void initialize(
        uint8_t anodePin0,
        uint8_t anodePin1,
        uint8_t anodePin2,
        uint8_t anodePin3,
        uint8_t anodePin4,
        uint8_t bcdPin0,
        uint8_t bcdPin1,
        uint8_t bcdPin2,
        uint8_t bcdPin3,
        NixieDigits_s *digits);

    /* 
     * Set the pointer to the Nixie digits structure
     * Parameters:
     *   digits     : array of Nixie tube display digits
     */
    void setDigits(NixieDigits_s *digits);

    /* 
     * Refresh the display
     * This function must be called from within a very fast loop
     */
    void refresh(void);

    /*
     * Force blink all digits disregarding the individual digit selecten
     * Parameters:
     *   enable : enable/disable blinking
     */
    void blinkAll(bool enable);

    /*
     * Resets the phase of digit blinking
     */
    void resetBlinking(void);

    /*
     * Trigger the "Slot Machine" effect
     */
    void slotMachine(void);

    /*
     * Exectue cathode poisoniong prevention sequence
     */
    void cathodePoisonPrevent(void);

    /*
     * Convert a decimal value to Nixie digits BCD format
     * Parameters:
     *   value     : value to be converted
     *   output    : pointer to the Nixie digits structure structure
     *   numDigits : number of BCD digits to be converted
     */
    void dec2bcd(uint32_t value, NixieDigits_s *output, uint8_t numDigits);

    /*
     * Reset a Nixie Digit buffer
     * Parameters:
     *   output : pointer to the Nixie digits structure structure
     */
    void resetDigits(NixieDigits_s *output);

    /*
     * Temporarily Blank the display by turning-off all the Anodes
     * will re-activate the next time refresh() is called
     */
    void blank(void);

    /*
     * Permanently Enable/disable display output
     * Parameters:
     *   enbale : enable/disable display
     */
    void enable(bool enable);

    /*
     * Tells if display is enabled or not
     */
    volatile bool enabled = true;

    /*
     * Pointer to the Nixie digits structure
     */
    NixieDigits_s *digits;

    /*
     * Status of the CPP feature
     */
    bool cppEnabled = false;

   private:
    int numTubes = NIXIE_NUM_TUBES;
    uint8_t anodePin[NIXIE_NUM_TUBES];
    uint8_t bcdPin[NIXIE_NUM_TUBES - 1];
    uint32_t digitOnDuration;
    uint32_t digitPeriod;
    uint32_t lastTs = 0;
    uint8_t digit = 0;
    bool blinkAllEnabled = false;
    uint32_t blinkTs = 0;
    bool blinkFlag = false;
    bool slotMachineEnabled[NIXIE_NUM_TUBES - 1] = {false};
    uint32_t slotMachineTs[NIXIE_NUM_TUBES - 1] = {0};
    uint8_t slotMachineCnt[NIXIE_NUM_TUBES - 1] = {0};
    uint32_t slotMachineDelay[NIXIE_NUM_TUBES - 1] = {0};
    uint8_t slotMachineCntStart[NIXIE_NUM_TUBES - 1] = {0, 11, 5, 13};
    uint8_t slotMachineCntMax[NIXIE_NUM_TUBES - 1] = {20, 50, 30, 60};
    uint32_t cppTs = 0;
    uint8_t cppCnt = 0;
};

/*
 * Nixie display class as singleton
 */
extern NixieClass Nixie;

#endif  // __NIXIE_H