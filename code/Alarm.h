/*
 * Alarm clock implementation class
 */

#ifndef __ALARM_H
#define __ALARM_H

class AlarmClass : public ChronoClass {
   public:
    void initialize();
    void resetAlarm(void);
    void incrementRight(void) volatile;
    void decrementRight(void) volatile;
    void incrementLeft(void) volatile;
    void decrementLeft(void) volatile;

    bool alarm = false;
    bool active = false;

   private:
    volatile Timekeeper tm;
};

#endif