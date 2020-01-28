/*
* Chrono class to keep track of time
*/

#include "Chrono.h"


void ChronoClass::increment10th (void) volatile {
  tenth++;
  if (tenth > 9) tenth = 0, second++;
  if (second > 59) second = 0, minute++;
  if (minute > 59) minute = 0, hour++;
}

void ChronoClass::increment10sec (void) volatile {
  second += 10;
  if (second > 59) second = 0, minute++;
  if (minute > 59) minute = 0, hour++;
}

bool ChronoClass::decrement10sec (void) volatile {
  second -= 10;
  if (second < 0) second = 59, minute--;
  if (minute < 0) minute = 59, hour--;
  if (hour < 0) hour = 0, minute = 0, second = 0;
  if (hour == 0 && minute == 0 && second == 0) return true;
  return false;
}

void ChronoClass::incrementMin (void) volatile {
  minute++;
  if (minute > 59) minute = 0, hour++;
}

bool ChronoClass::decrementMin (void) volatile {
  minute--;
  if (minute < 0) minute = 59, hour--;
  if (hour < 0) hour = 0, minute = 0, second = 0;
  if (hour == 0 && minute == 0 && second == 0) return true;
  return false;
}

void ChronoClass::incrementSec (void) volatile {
  second++;
  if (second > 59) second = 0, minute++;
  if (minute > 59) minute = 0, hour++;
}

bool ChronoClass::decrementSec (void) volatile {
  second--;
  if (second < 0) second = 59, minute--;
  if (minute < 0) minute = 59, hour--;
  if (hour < 0) hour = 0, minute = 0, second = 0;
  if (hour == 0 && minute == 0 && second == 0) return true;
  return false;
}

void ChronoClass::reset (void) volatile {
  tenth = 0;
  second = 0;
  minute = 0;
  hour = 0;
}

void ChronoClass::copy (volatile ChronoClass *tm) volatile {
  tenth = tm->tenth;
  second = tm->second;
  minute = tm->minute;
  hour = tm->hour;
}

void ChronoClass::roundup (void) volatile {
  tenth = 0;
  if (second !=0) second = 0, minute++;
  if (minute > 59) minute = 0, hour++;
}