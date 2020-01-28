/*
 * Countdown timer implementation class
 */

#ifndef __TIMER_H
#define __TIMER_H

class TimerClass {
  public:
    void initialize (void (*callback)(bool start));
    void loopHandler (void);
    void tick (void);
    void secondIncrease (void);
    void secondDecrease (void);
    void minuteIncrease (void);
    void minuteDecrease (void);
    void displayRefresh (void);
    void start (void);
    void stop (void);
    void resetAlarm (void);
    void reset (void);
    
    volatile bool active = false;
    volatile bool running = false;
    volatile bool alarm = false;
    NixieDigits_s digits; 
  
  private:
    uint32_t alarmTs = 0;
    ChronoClass defaultTm;
    volatile ChronoClass tm;
    volatile bool tickFlag = false; 
    void (*callback)(bool) = NULL;
};
