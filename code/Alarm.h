#ifndef __ALARM_H
#define __ALARM_H

/*
 * Alarm modes
 */
enum AlarmMode_e {
  ALARM_OFF = 0,
  ALARM_WEEKENDS = 2,
  ALARM_WEEKDAYS = 5,
  ALARM_DAILY = 7
};

/*
 * Alarm settings to be stored in EEPROM
 */
struct AlarmEeprom_s {
  int8_t hour;
  int8_t minute;
  AlarmMode_e mode;
  AlarmMode_e lastMode;
};

/*
 * Alarm clock implementation class
 */
class AlarmClass {
  public:
    void initialize (AlarmEeprom_s * settings);
    void loopHandler (int8_t hour, int8_t minute, int8_t wday, bool active);
    void startAlarm (void);
    void snooze (void);
    void resetAlarm (void);
    void modeIncrease (void); 
    void modeDecrease (void);
    void modeToggle (void);
    void minuteIncrease (void);
    void minuteDecrease (void);
    void hourIncrease (void);
    void hourDecrease (void);
    void displayRefresh (void);

    bool alarm = false;
    bool snoozing = false;
    NixieDigits_s digits;
    AlarmEeprom_s *settings = NULL;

  private:
    uint32_t snoozeTs = 0; 
    uint32_t alarmTs = 0;    
    int8_t lastMinute = 0;
    uint32_t blinkTs = 0;
    bool alarmCondition = false;
};