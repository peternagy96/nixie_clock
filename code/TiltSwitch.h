/*
 * Push button implementation class
 */

#ifndef __TILTSWITCH_H
#define __TILTSWITCH_H

class TiltSwitchClass
{
public:
    void up(void);
    void down(void);
    void middle(void);

    bool up = false;
    bool down = false;
    bool middle = false;

private:
};