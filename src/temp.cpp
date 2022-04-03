#include <Arduino.h>

#include "config.h"

int temp_int_mv;
double temp_int;
int temp_ext_mv;
double temp_ext;

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
    int vin = analogRead(temp_pin);
    digitalWrite(enable_pin, LOW);
    return vin;
}

double mv_to_c_helper(int mv) {
    // convert from internal precision to mv then apply formula in datasheet
    return (((double) mv * 1100 / 1024.) - 424) / 6.25;
}

bool read_temp() {
    temp_int_mv = read_temp_helper(PIN_TEMP_INT_EN, PIN_TEMP_INT);
    temp_ext_mv = read_temp_helper(PIN_TEMP_EXT_EN, PIN_TEMP_EXT);
    temp_int = mv_to_c_helper(temp_int_mv);
    temp_ext = mv_to_c_helper(temp_ext_mv);
#ifdef DEBUG
    DEBUG_STREAM.print(F("temp_int_mv: "));
    DEBUG_STREAM.println(temp_int_mv);
    DEBUG_STREAM.print(F("temp_int: "));
    DEBUG_STREAM.println(temp_int);
    DEBUG_STREAM.print(F("temp_ext_mv: "));
    DEBUG_STREAM.println(temp_ext_mv);
    DEBUG_STREAM.print(F("temp_ext: "));
    DEBUG_STREAM.println(temp_ext);
#endif
    return true;
}
