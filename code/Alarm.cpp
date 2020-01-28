/*
* Alarm class to implement features for 2 independent alarms
*/

#include "Alarm.h"

void AlarmClass::initialize (AlarmEeprom_s *settings) {
  this->settings = settings;
  alarm = false;
  snoozing = false;
  Nixie.blinkAll (false);
  if (settings->minute < 0 || settings->minute > 59) settings->minute = 0;
  if (settings->hour < 0 || settings->hour > 23) settings->hour = 0;
  if (settings->mode != ALARM_OFF && settings->mode != ALARM_WEEKENDS && 
      settings->mode != ALARM_WEEKDAYS && settings->mode != ALARM_DAILY) settings->mode = ALARM_OFF;
  if (settings->lastMode != ALARM_OFF && settings->lastMode != ALARM_WEEKENDS && 
      settings->lastMode != ALARM_WEEKDAYS && settings->lastMode != ALARM_DAILY) settings->lastMode = ALARM_DAILY;
  displayRefresh ();
}

void AlarmClass::loopHandler (int8_t hour, int8_t minute, int8_t wday, bool active) {
  uint32_t ts = millis();

  if (minute != lastMinute) alarmCondition = false;
  
  if (active && !snoozing && settings->mode != ALARM_OFF && !alarmCondition &&
      minute == settings->minute && hour == settings->hour && 
      (settings->mode != ALARM_WEEKDAYS || (wday >= 1 && wday <= 5)) && 
      (settings->mode != ALARM_WEEKENDS || wday == 0 || wday == 6)) {
    startAlarm (); 
    alarmCondition = true;
  }

  if (snoozing && ts - snoozeTs > ALARM_SNOOZE_DURATION) startAlarm ();

  if (snoozing && ts - blinkTs > 500) {
    Nixie.comma[0] = !Nixie.comma[0];
    blinkTs = ts;
  }

  // Alarm timout
  // IMPORTANT: use millis() not ts to avoid premature alarm reset
  // due to a boundary condition where alarmTs > ts
  // (alarmTs is set within startAlarm() which is called after ts was assigned)
  if (alarm && millis() - alarmTs > ALARM_ALARM_DURATION) resetAlarm ();

  lastMinute = minute;
}

void AlarmClass::startAlarm (void) {
  if (!alarm) {
    alarm = true;
    Nixie.resetBlinking ();
    Nixie.blinkAll (true);
    Buzzer.playMelody1 ();
    alarmTs = millis ();
    snoozing = false;
    displayRefresh ();
  }
}

void AlarmClass::snooze (void) {
  if (alarm && !snoozing) {
    alarm = false;
    Nixie.blinkAll (false);
    Buzzer.stop ();
    snoozing = true;
    snoozeTs = millis ();
    displayRefresh ();
  }
}

void AlarmClass::resetAlarm (void) {
  if (alarm || snoozing) {
    alarm = false;
    Nixie.blinkAll (false);
    Buzzer.stop ();
    snoozing = false;
    displayRefresh ();
  }
}

void AlarmClass::modeIncrease (void) {
  if      (settings->mode == ALARM_OFF)      settings->mode = ALARM_WEEKENDS;
  else if (settings->mode == ALARM_WEEKENDS) settings->mode = ALARM_WEEKDAYS;
  else if (settings->mode == ALARM_WEEKDAYS) settings->mode = ALARM_DAILY;
  else if (settings->mode == ALARM_DAILY)    settings->mode = ALARM_OFF, settings->lastMode = ALARM_DAILY;
  displayRefresh ();
}

void AlarmClass::modeDecrease (void) {
  if      (settings->mode == ALARM_OFF)      settings->mode = ALARM_DAILY;
  else if (settings->mode == ALARM_DAILY)    settings->mode = ALARM_WEEKDAYS;
  else if (settings->mode == ALARM_WEEKDAYS) settings->mode = ALARM_WEEKENDS;
  else if (settings->mode == ALARM_WEEKENDS) settings->mode = ALARM_OFF, settings->lastMode = ALARM_DAILY;
  displayRefresh ();
}

void AlarmClass::modeToggle (void) {
  if (settings->mode == ALARM_OFF) {
    settings->mode = settings->lastMode;
  }
  else {
    settings->lastMode = settings->mode;
    settings->mode = ALARM_OFF;
  }
  displayRefresh ();
}

void AlarmClass::minuteIncrease (void) {
  settings->minute++;
  if (settings->minute > 59) settings->minute = 0;
  displayRefresh ();
}

void AlarmClass::minuteDecrease (void) {
  settings->minute--;
  if (settings->minute < 0) settings->minute = 59;
  displayRefresh ();
}

void AlarmClass::hourIncrease (void) {
  settings->hour++;
  if (settings->hour > 23) settings->hour = 0;
  displayRefresh ();
}

void AlarmClass::hourDecrease (void) {
  settings->hour--;
  if (settings->hour < 0) settings->hour = 23;
  displayRefresh ();
}

void AlarmClass::displayRefresh (void) {
  for (uint8_t i = 0; i < 6; i++) digits.blank[i] = false;
  digits.value[0] = (uint8_t)settings->mode;
  Nixie.comma[0] = (bool)settings->mode;
  digits.blank[1] = true;
  digits.value[2] = dec2bcdLow  (settings->minute);
  digits.value[3] = dec2bcdHigh (settings->minute);
  digits.value[4] = dec2bcdLow  (settings->hour);
  digits.value[5] = dec2bcdHigh (settings->hour);   
}
