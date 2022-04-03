#include "buzzer.h"

#include <Arduino.h>

#include "config.h"

bool buzzer_state = false;

bool setup_buzz() {
    pinMode(PIN_BUZZER, OUTPUT);
    return true;
}

bool is_buzz_time(double altitude) {
    bool c_is_buzz_time = (altitude < BUZZ_ALT);
#ifdef DEBUG
    DEBUG_STREAM.print(F("is_buzz_time: "));
    DEBUG_STREAM.println(c_is_buzz_time);
#endif
    return c_is_buzz_time;
}

void disable_buzzer() {
    noTone(PIN_BUZZER);
}

void flip_buzz_state() {
#ifdef DEBUG
    DEBUG_STREAM.print(F("buzz_state: "));
    DEBUG_STREAM.println(buzzer_state);
#endif
    if (buzzer_state) {
        tone(PIN_BUZZER, 1500);
    } else {
        disable_buzzer();
    }
    buzzer_state = !buzzer_state;
}
