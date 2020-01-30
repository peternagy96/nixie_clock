/*
 * Push button implementation class
 */

#ifndef __TILTSWITCH_H
#define __TILTSWITCH_H

#include <stdint.h>
#include "Chrono.h"

class TiltSwitchClass {
   public:
    void setToUp(void);
    void setToDown(void);
    void setToMiddle(void);
    void incrementRight(ChronoClass* chrono);
    void decrementRight(ChronoClass* chrono);
    void incrementLeft(ChronoClass* chrono);
    void decrementLeft(ChronoClass* chrono);

    bool up = false;
    bool down = false;
    bool middle = false;

   private:
};

#endif