#ifndef GPS_H
#define GPS_H

#include <Arduino.h>

extern double gps_speed;
extern double gps_altitude;
extern double gps_course;
extern double gps_longitude;
extern double gps_latitude;
extern int gps_time;
extern int gps_age;
extern int gps_sat_count;

bool setup_gps();
void sleep_read_gps(uint32_t ms);
void parse_gps();

#endif
