// Helper functions

#include "Helper.h"

void updateDigits()
{
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

    if (G.menuState == SHOW_TIME)
    {
        // trigger Nixie digit "Slot Machine" effect
        if (G.systemTm->tm_min != lastMin && (Settings.cathodePoisonPrevent == 2 || (Settings.cathodePoisonPrevent == 3 && G.timeDigits.value[2] == 0)))
        {
            Nixie.slotMachine();
        }
        // trigger the cathode poisoning prevention routine
        if (Settings.cathodePoisonPrevent == 1 && G.cppEffectEnabled && G.timeDigits.value[0] == 0)
        {
            Nixie.cathodePoisonPrevent();
        }
    }
    lastMin = G.systemTm->tm_min;
}