/*
 * Push button implementation class
 */

#ifndef __PUSHBUTTON_H
#define __PUSHBUTTON_H

class PushButtonClass {
   public:
    void press(void);
    void release(void);
    bool rising(void);
    bool falling(void);
    bool fallingLongPress(void);
    bool fallingContinuous(void);
    bool longPress(void);

    bool pressed = false;
    bool wasPressed = false;

   private:
    bool longPressed = false;
    bool wasLongPressed = false;
    uint32_t longPressTs = 0;
};

#endif