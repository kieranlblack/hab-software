#include "button.h"

#include <Arduino.h>

#include "buzzer.h"
#include "config.h"

volatile unsigned long last_time = 0;
volatile bool is_rising_edge_last = false;

bool setup_button() {
    pinMode(PIN_BUTTON, INPUT);
    attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), button_isr, CHANGE);
    return true;
}

void button_isr() {
    bool is_rising_edge = digitalRead(PIN_BUTTON) == HIGH;
    uint32_t curr_time = millis();
    uint32_t time_delta = curr_time - last_time;

    if (is_rising_edge) {
        // the interrupt handler can register two rising edges in a row with an arbitrary time gap inbetween
        if (!is_rising_edge_last && time_delta > DEBOUNCE_MS) {
            #ifdef DEBUG
                DEBUG_STREAM.println(F("button interrupt recieved"));
                DEBUG_STREAM.println(curr_time - last_time);
            #endif
            toggle_buzzer_enable();
            last_time = curr_time;
        }
    } else {
        // if someone held down the button then we want to ignore bounces from releasing it
        if (time_delta > 250) {
            last_time = curr_time;
        }
    }
    is_rising_edge_last = is_rising_edge;
}
