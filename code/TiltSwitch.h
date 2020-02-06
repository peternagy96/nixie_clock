/*
 * Push button implementation class
 */

#ifndef __TILTSWITCH_H
#define __TILTSWITCH_H

#include <time.h>
#include "Chrono.h"

class TiltSwitchClass {
   public:
    void setPin(uint8_t upPin, uint8_t downPin);
    void readState(void);
    void setToUp(void);
    void setToDown(void);
    void setToMiddle(void);
    void incrementHour(ChronoClass* chrono);
    void decrementHour(ChronoClass* chrono);
    void incrementMin(ChronoClass* chrono);
    void decrementMin(ChronoClass* chrono);

    uint8_t upPin;
    uint8_t downPin;
    bool up = false;
    bool down = false;
    bool middle = false;

   private:
};

#endif