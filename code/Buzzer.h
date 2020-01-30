#ifndef __BUZZER_H
#define __BUZZER_H

/*
 * Buzzer control class
 */
class BuzzerClass {
   public:
    void initialize(uint8_t buzzerPin);
    void loopHandler(void);
    void playMelody1(void);
    void playMelody2(void);
    void stop(void);

    bool active = false;

   private:
    bool initialized = false;
    uint8_t buzzerPin;
    uint8_t index = 0;
    uint32_t melodyTs = 0;
    int32_t *melody = melody1;
    //int32_t melody1[9] = { 500, 50, 100, 50, 100, 50, 100, 50, -1 };
    int32_t melody1[9] = {1450, 50, 200, 50, 200, 50, -1};
    int32_t melody2[3] = {950, 50, -1};
};

/*
 * Buzzer object as a singleton
 */
extern BuzzerClass Buzzer;