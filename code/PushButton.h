/*
 * Push button implementation class
 */

#ifndef __PUSHBUTTON_H
#define __PUSHBUTTON_H

#include <stdint.h>

class PushButtonClass {
   public:
    void setPin(uint8_t pin);
    void readState(void);
    void press(void);
    void release(void);
    bool rising(void);
    bool falling(void);
    bool longPress(void);
    bool reallyLongPress(void);

    bool pressed = false;
    bool wasPressed = false;

   private:
    uint8_t pin;
    bool longPressed = false;
    bool reallyLongPressed = false;
    bool wasLongPressed = false;
    bool wasReallyLongPressed = false;
    uint32_t longPressTs = 0;
};

#endif