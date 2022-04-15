#include <Arduino.h>

#include "button.h"
#include "buzzer.h"
#include "config.h"
#include "dht20_sensor.h"
#include "double_log_component.h"
#include "gps.h"
#include "int_log_component.h"
#include "lm60.h"
#include "log.h"
#include "sdcard.h"
#include "uint32_log_component.h"
#include "voltage.h"

Log my_log;

uint32_t system_time = millis();
UInt32LogComponent system_time_component(&system_time, "sys_time");

DoubleLogComponent temp_dht_component(&temp_dht, "temp_dht");
DoubleLogComponent humidity_component(&humidity, "hum");

UInt32LogComponent voltage_component(&voltage, "mv");
DoubleLogComponent temp_int_component(&temp_int, "temp_int");
IntLogComponent temp_int_mv_component(&temp_int_mv, "temp_int_mv");
DoubleLogComponent temp_ext_component(&temp_ext, "temp_ext");
IntLogComponent temp_ext_mv_component(&temp_ext_mv, "temp_ext_mv");

UInt32LogComponent gps_sat_count_component(&gps_sat_count, "sat_count");
UInt32LogComponent gps_time_component(&gps_time, "gps_time");
UInt32LogComponent gps_age_component(&gps_age, "age");
DoubleLogComponent gps_speed_component(&gps_speed, "speed");
DoubleLogComponent gps_altitude_component(&gps_altitude, "altitude");
DoubleLogComponent gps_course_component(&gps_course, "course");
DoubleLogComponent gps_longitude_component(&gps_longitude, "longitude");
DoubleLogComponent gps_latitude_component(&gps_latitude, "latitude");

void setup() {
    #ifdef DEBUG
        DEBUG_STREAM.begin(9600);
        DEBUG_STREAM.println(F("running setup"));
    #endif
    // setup in no particular order
    setup_sd();
    setup_gps();
    setup_dht20();
    setup_temp();
    setup_buzz();
    setup_button();

    #ifdef DEBUG
        DEBUG_STREAM.println(F("registering variables"));
    #endif
    // register all variables to the log
    my_log.register_log_component(&system_time_component);

    my_log.register_log_component(&temp_int_component);
    my_log.register_log_component(&temp_int_mv_component);
    my_log.register_log_component(&temp_ext_component);
    my_log.register_log_component(&temp_ext_mv_component);

    my_log.register_log_component(&temp_dht_component);
    my_log.register_log_component(&humidity_component);

    my_log.register_log_component(&gps_sat_count_component);
    my_log.register_log_component(&gps_time_component);
    my_log.register_log_component(&gps_age_component);
    my_log.register_log_component(&gps_speed_component);
    my_log.register_log_component(&gps_altitude_component);
    my_log.register_log_component(&gps_course_component);
    my_log.register_log_component(&gps_longitude_component);
    my_log.register_log_component(&gps_latitude_component);

    my_log.register_log_component(&voltage_component);

    #ifdef DEBUG
        DEBUG_STREAM.println(F("generating headers"));
    #endif
    // generate headers
    #ifdef DEBUG
        my_log.write_log(DEBUG_STREAM, true);
    #endif
    log_to_sd(my_log, true);

    sleep_read_gps(2000);
}

void loop() {
    #ifdef DEBUG
        DEBUG_STREAM.println(F("===================="));
    #endif
    system_time = millis();
    parse_gps();
    read_dht20();
    read_lm60();
    read_voltage();

    #ifdef DEBUG
        my_log.write_log(DEBUG_STREAM, true);
        my_log.write_log(DEBUG_STREAM, false);
    #endif
    log_to_sd(my_log, false);

    if (is_buzz_time(gps_altitude)) {
        flip_buzz_state();
    } else {
        tone_off_buzzer();
    }

    sleep_read_gps(2000);
}
