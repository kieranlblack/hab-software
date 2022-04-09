#include <Arduino.h>

#include "buzzer.h"
#include "button.h"
#include "config.h"
#include "double_log_component.h"
#include "gps.h"
#include "int_log_component.h"
#include "log.h"
#include "mprls.h"
#include "sdcard.h"
#include "temp.h"
#include "uint32_log_component.h"

Log my_log;

uint32_t system_time = millis();
UInt32LogComponent system_time_component(&system_time, "T");

DoubleLogComponent temp_int_component(&temp_int, "ti");
IntLogComponent temp_int_mv_component(&temp_int_mv, "timv");
DoubleLogComponent temp_ext_component(&temp_ext, "te");
IntLogComponent temp_ext_mv_component(&temp_ext_mv, "temv");

DoubleLogComponent pressure_component(&pressure, "pres");

UInt32LogComponent gps_sat_count_component(&gps_sat_count, "sc");
UInt32LogComponent gps_time_component(&gps_time, "gT");
UInt32LogComponent gps_age_component(&gps_age, "age");
DoubleLogComponent gps_speed_component(&gps_speed, "sp");
DoubleLogComponent gps_altitude_component(&gps_altitude, "alt");
DoubleLogComponent gps_course_component(&gps_course, "crs");
DoubleLogComponent gps_longitude_component(&gps_longitude, "long");
DoubleLogComponent gps_latitude_component(&gps_latitude, "lat");

void setup() {
    #ifdef DEBUG
        DEBUG_STREAM.begin(9600);
        DEBUG_STREAM.println(F("running setup"));
    #endif
    // setup in no particular order
    setup_sd();
    setup_gps();
    // setup_mprls();
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

    my_log.register_log_component(&pressure_component);

    my_log.register_log_component(&gps_sat_count_component);
    my_log.register_log_component(&gps_time_component);
    my_log.register_log_component(&gps_age_component);
    my_log.register_log_component(&gps_speed_component);
    my_log.register_log_component(&gps_altitude_component);
    my_log.register_log_component(&gps_course_component);
    my_log.register_log_component(&gps_longitude_component);
    my_log.register_log_component(&gps_latitude_component);

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
    // read_mprls();
    read_temp();

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
