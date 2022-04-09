#include "lm60.h"

#include <Arduino.h>

#include "config.h"

int temp_int_mv = 0;
double temp_int = 0;
int temp_ext_mv = 0;
double temp_ext = 0;

int read_temp_helper(uint8_t enable_pin, uint8_t temp_pin);

bool setup_temp() {
    pinMode(PIN_TEMP_EXT_EN, OUTPUT);
    pinMode(PIN_TEMP_INT_EN, OUTPUT);
    return true;
}

int read_temp_helper(uint8_t enable_pin, uint8_t temp_pin) {
    digitalWrite(enable_pin, HIGH);
    analogReference(INTERNAL);
    analogRead(temp_pin);
    delay(10);
    int adc = analogRead(temp_pin);
    digitalWrite(enable_pin, LOW);
    // convert from internal precision to mv
    return 1100. * adc / 1024.;
}

double mv_to_c_helper(int mv) {
    // apply formula in datasheet
    return ((double) mv - 424) / 6.25;
}

bool read_lm60() {
    temp_int_mv = read_temp_helper(PIN_TEMP_INT_EN, PIN_TEMP_INT);
    temp_ext_mv = read_temp_helper(PIN_TEMP_EXT_EN, PIN_TEMP_EXT);
    temp_int = mv_to_c_helper(temp_int_mv);
    temp_ext = mv_to_c_helper(temp_ext_mv);
    return true;
}
