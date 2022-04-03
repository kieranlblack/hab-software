#include "gps.h"

#include <AltSoftSerial.h>
#include <TinyGPS++.h>

double gps_speed = 0;
double gps_altitude = 0;
double gps_course = 0;
double gps_longitude = 0;
double gps_latitude = 0;
int gps_time = 0;
int gps_age = 0;
int gps_sat_count;

AltSoftSerial ss;
TinyGPSPlus gps;

bool setup_gps() {
    ss.begin(9600);
    return true;
}

void sleep_read_gps(uint32_t ms) {
    uint32_t start_time = millis();
    do {
        while (ss.available() > 0) gps.encode(ss.read());
    } while (millis() - start_time < ms);
}

void parse_gps() {
    if (gps.location.isValid()) {
        gps_latitude = gps.location.lat();
        gps_longitude = gps.location.lng();
        gps_age = gps.location.age();
    }
    if (gps.altitude.isValid()) {
        gps_altitude = gps.altitude.meters();
    }
    if (gps.speed.isValid()) {
        gps_speed = gps.speed.mps();
    }
    if (gps.satellites.isValid()) {
        gps_sat_count = gps.satellites.value();
    }
    if (gps.course.isValid()) {
        gps_course = gps.course.deg();
    }
    if (gps.time.isValid()) {
        gps_time = gps.time.value();
    }
}
