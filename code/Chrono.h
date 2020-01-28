/*
* Chrono class to keep track of time
*/

#ifndef __CHRONO_H
#define __CHRONO_H

/*
 * Class for performing simple timekeeping operations
 * can be used for a chronometer or countdown timer
 */
class ChronoClass {
  public:
    void increment10th (void) volatile;
    void increment10sec (void) volatile;
    bool decrement10sec (void) volatile;
    void incrementMin (void) volatile;
    bool decrementMin (void) volatile;
    void incrementSec (void) volatile;
    bool decrementSec (void) volatile;
    void reset (void) volatile;
    void copy (volatile ChronoClass *) volatile;
    void roundup (void) volatile;

    volatile int8_t tenth = 0;
    volatile int8_t second = 0;
    volatile int8_t minute = 0;
    volatile int8_t hour = 0;
    
  private:
};