/*
* Timer class to implement countdown timer
*/

#include "Timer.h"

void TimerClass::initialize (void (*callback)(bool)) {
  this->callback = callback;
  defaultTm.tenth = 0;
  defaultTm.second = 0;
  defaultTm.minute = 5;
  defaultTm.hour = 0;
  reset ();
}

void TimerClass::loopHandler (void) {
  uint32_t ts = millis ();
  
  if (tickFlag) { 
    if (!alarm) {
      alarm = tm.decrementSec (); 
      if (alarm) {
        //stop ();
        alarmTs = ts;
        tm.copy (&defaultTm);
        Nixie.resetBlinking ();
        Nixie.blinkAll (true);
        Buzzer.playMelody2 ();
      }
    }
    else {
      tm.incrementSec ();
    }
    displayRefresh ();
    tickFlag = false;
  }
  
  if (alarm && ts - alarmTs > TIMER_ALARM_DURATION) resetAlarm ();
}

void TimerClass::tick (void) {
  if (running) {  
    tickFlag = true;
  }
}

void TimerClass::secondIncrease (void) {
  stop ();
  tm.increment10sec ();
  displayRefresh ();
  start ();
}

void TimerClass::secondDecrease (void) {
  bool rv;
  stop ();
  rv = tm.decrement10sec ();
  displayRefresh ();
  if (rv) stop ();
  else start ();
}

void TimerClass::minuteIncrease (void) {
  stop ();
  tm.incrementMin ();
  displayRefresh ();
  start ();
}

void TimerClass::minuteDecrease (void) {
  bool rv;
  stop ();
  rv = tm.decrementMin ();
  displayRefresh ();
  if (rv) stop ();
  else start ();
}

void TimerClass::displayRefresh (void) {
  digits.value[0] = dec2bcdLow  (tm.second);
  digits.value[1] = dec2bcdHigh (tm.second);
  digits.value[2] = dec2bcdLow  (tm.minute);
  digits.value[3] = dec2bcdHigh (tm.minute);
  digits.value[4] = dec2bcdLow  (tm.hour);
  digits.value[5] = dec2bcdHigh (tm.hour); 
}

void TimerClass::start (void) {
  defaultTm.copy (&tm);
  active = true;
  if (!running) {
    running = true;
    callback (true);
  }  
}

void TimerClass::stop (void) {
  if (running) {
    running = false;
    callback (false);    
  }
}

void TimerClass::resetAlarm (void) {
  if (alarm) {
    alarm = false;
    Nixie.blinkAll (false);
    Buzzer.stop ();
    stop ();
  }
}

void TimerClass::reset (void) {
  resetAlarm ();
  active = false;
  running = false;
  Nixie.resetDigits (&digits);
  callback (false);
  defaultTm.roundup ();
  tm.copy (&defaultTm);
  displayRefresh ();
}