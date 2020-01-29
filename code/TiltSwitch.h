/*
 * Push button implementation class
 */

#ifndef __TILTSWITCH_H
#define __TILTSWITCH_H

class TiltSwitchClass {
   public:
    void up(void);
    void down(void);
    void middle(void);
    void incrementRight(ChronoClass &chrono);
    void decrementRight(ChronoClass &chrono);
    void incrementLeft(ChronoClass &chrono);
    void decrementLeft(ChronoClass &chrono);

    bool up = false;
    bool down = false;
    bool middle = false;

   private:
};