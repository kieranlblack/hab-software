#include "buzzer.h"

#include <Arduino.h>

#include "config.h"

volatile bool enable_buzzer = false;
bool buzzer_state = false;

bool setup_buzz() {
    pinMode(PIN_BUZZER_MOSFET, OUTPUT);
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

void toggle_buzzer_enable() {
    enable_buzzer = !enable_buzzer;
}

void disable_buzzer() {
    digitalWrite(PIN_BUZZER_MOSFET, LOW);
}

void flip_buzz_state() {
    #ifdef DEBUG
        DEBUG_STREAM.print(F("buzz_state: "));
        DEBUG_STREAM.println(buzzer_state);
        DEBUG_STREAM.print(F("enable_buzzer: "));
        DEBUG_STREAM.println(enable_buzzer);
    #endif
    if (buzzer_state && enable_buzzer) {
        digitalWrite(PIN_BUZZER_MOSFET, HIGH);
    } else {
        disable_buzzer();
    }
    buzzer_state = !buzzer_state;
}
