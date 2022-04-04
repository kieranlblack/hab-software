#ifndef GPS_H
#define GPS_H

#include <Arduino.h>

extern double gps_speed;
extern double gps_altitude;
extern double gps_course;
extern double gps_longitude;
extern double gps_latitude;
extern uint32_t gps_time;
extern uint32_t gps_age;
extern uint32_t gps_sat_count;

bool setup_gps();
void sleep_read_gps(uint32_t ms);
void parse_gps();

#endif
