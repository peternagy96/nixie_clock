/*
 * Countdown timer implementation class
 */

#ifndef __TIMER_H
#define __TIMER_H

class TimerClass : public ChronoClass {
   public:
    void initialize();
    void increaseRight(void) volatile;
    void decreaseRight(void) volatile;
    void increaseLeft(void) volatile;
    void decreaseLeft(void) volatile;

    void start(void);
    void stop(void);
    void resetAlarm(void);
    void reset(void);

    volatile bool running = false;
    volatile bool alarm = false;

   private:
    uint32_t alarmTs = 0;  // ToDo: implement timeout for the alarm
    Timekeeper defaultTm;
    volatile Timekeeper tm;
};
