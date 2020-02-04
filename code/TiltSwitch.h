/*
 * Push button implementation class
 */

#ifndef __TILTSWITCH_H
#define __TILTSWITCH_H

#include <stdint.h>
#include "Chrono.h"

class TiltSwitchClass {
   public:
    void setPin(uint8_t upPin, uint8_t downPin);
    void readState(void);
    void setToUp(void);
    void setToDown(void);
    void setToMiddle(void);
    void incrementRight(ChronoClass* chrono);
    void decrementRight(ChronoClass* chrono);
    void incrementLeft(ChronoClass* chrono);
    void decrementLeft(ChronoClass* chrono);

    uint8_t upPin;
    uint8_t downPin;
    bool up = false;
    bool down = false;
    bool middle = false;

   private:
};

#endif