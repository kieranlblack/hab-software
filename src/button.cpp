#include "buzzer.h"
#include "config.h"

#include <Arduino.h>

#define toggle_switchbounce_time_ms 250

unsigned long button_time = 0;
unsigned long last_button_time = 0;
unsigned long last_button_low_time = 0;

void toggle_buzzer() {
  button_time = millis();
  if (digitalRead(PIN_BUTTON) == LOW) {
    last_button_low_time = millis();
  }
  if (button_time - last_button_time > 250 && button_time - last_button_low_time > 250){
    #ifdef DEBUG
    DEBUG_STREAM.println("Interrupt ");
    #endif
    toggle_buzzer_enable();
    last_button_time = button_time;
  }
}
