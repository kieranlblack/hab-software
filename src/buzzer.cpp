#include "buzzer.h"

#include <Arduino.h>

#include "config.h"

volatile bool enable_buzzer = false;
bool buzzer_state = false;

bool setup_buzz() {
    pinMode(PIN_BUZZER_MOSFET, OUTPUT);
    tone_off_buzzer();
    return true;
}

bool is_buzz_time(double altitude) {
    bool c_is_buzz_time = (altitude < BUZZ_ALT);
    #ifdef DEBUG_BUZZ_BUTT
        DEBUG_STREAM.print(F("is_buzz_time: "));
        DEBUG_STREAM.println(c_is_buzz_time);
    #endif
    return c_is_buzz_time;
}

void toggle_buzzer_enable() {
    enable_buzzer = !enable_buzzer;
    if (!enable_buzzer) {
        tone_off_buzzer();
    }
}

void tone_off_buzzer() {
    // we have a pnp transistor, it turns on when there is no current at the base
    digitalWrite(PIN_BUZZER_MOSFET, HIGH);
}

void tone_on_buzzer() {
    digitalWrite(PIN_BUZZER_MOSFET, LOW);
}

void flip_buzz_state() {
    #ifdef DEBUG_BUZZ_BUTT
        DEBUG_STREAM.print(F("buzz_state: "));
        DEBUG_STREAM.println(buzzer_state);
        DEBUG_STREAM.print(F("enable_buzzer: "));
        DEBUG_STREAM.println(enable_buzzer);
    #endif
    if (buzzer_state && enable_buzzer) {
        tone_on_buzzer();
    } else {
        tone_off_buzzer();
    }
    buzzer_state = !buzzer_state;
}
