#include "gps.h"

#include <AltSoftSerial.h>
#include <TinyGPS++.h>

#include "config.h"

double gps_speed = 0;
double gps_altitude = 0;
double gps_course = 0;
double gps_longitude = 0;
double gps_latitude = 0;
uint32_t gps_time = 0;
uint32_t gps_age = 0;
uint32_t gps_sat_count = 0;

AltSoftSerial ss;
TinyGPSPlus gps;

// airborne 1g
const uint8_t set_flight_mode_msg[] PROGMEM =
{
    0xB5, 0x62, 0x06, 0x24, 0x24, 0x00, 0xFF, 0xFF, 0x06,
    0x03, 0x00, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00,
    0x05, 0x00, 0xFA, 0x00, 0xFA, 0x00, 0x64, 0x00, 0x2C,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0xDC
};

// all ublox code from http://ava.upuaut.net/
void sendUBX(const uint8_t *MSG, uint8_t len) {
    ss.flush();
    uint8_t temp;
    for (int i = 0; i < len; i++) {
        temp = pgm_read_byte(&MSG[i]);
        ss.write(temp);
        #ifdef DEBUG
            DEBUG_STREAM.print(temp, HEX);
        #endif
    }
    ss.println();
    #ifdef DEBUG
        DEBUG_STREAM.println(F("."));
        DEBUG_STREAM.println(F("message sent to gps"));
    #endif
}

bool getUBX_ACK(const uint8_t *MSG) {
    uint8_t b;
    uint8_t ackByteID = 0;
    uint8_t ackPacket[10];
    unsigned long startTime = millis();

    // Construct the expected ACK packet
    ackPacket[0] = 0xB5; // header
    ackPacket[1] = 0x62; // header
    ackPacket[2] = 0x05; // class
    ackPacket[3] = 0x01; // id
    ackPacket[4] = 0x02; // length
    ackPacket[5] = 0x00;
    ackPacket[6] = pgm_read_byte(&MSG[2]); // ACK class
    ackPacket[7] = pgm_read_byte(&MSG[3]); // ACK id
    ackPacket[8] = 0; // CK_A
    ackPacket[9] = 0; // CK_B

    // Calculate the checksums
    for (uint8_t ubxi = 2; ubxi < 8; ubxi++) {
        ackPacket[8] = ackPacket[8] + ackPacket[ubxi];
        ackPacket[9] = ackPacket[9] + ackPacket[8];
    }

    while (1) {
        // Test for success
        if (ackByteID > 9) {
            // All packets in order!
            return true;
        }

        // Timeout if no valid response in 3 seconds
        if (millis() - startTime > 3000) {
            #ifdef DEBUG
                DEBUG_STREAM.println(F("gps timed out"));
            #endif
            return false;
        }

        // Make sure data is available to read
        if (ss.available()) {
            b = ss.read();
            // Check that bytes arrive in sequence as per expected ACK packet
            if (b == ackPacket[ackByteID]) {
                ackByteID++;
            } else {
                ackByteID = 0; // Reset and look again, invalid order
            }
        }
    }
}

bool setup_gps() {
    ss.begin(9600);

    // set to flight mode, similar to the trackuino implementation
    int attempts_remaining = 15;
    bool gps_success = false;
    while (!gps_success && attempts_remaining > 0) {
        sendUBX(set_flight_mode_msg, sizeof(set_flight_mode_msg) / sizeof(uint8_t));
        gps_success = getUBX_ACK(set_flight_mode_msg);
        if (!gps_success) {
            #ifdef DEBUG
                DEBUG_STREAM.print(F("unable to set flight mode, "));
                DEBUG_STREAM.print(attempts_remaining - 1);
                DEBUG_STREAM.println(F(" attempts remaining"));
            #endif
            delay(500);
        }

        attempts_remaining--;
    }
    #ifdef DEBUG
        if (gps_success) {
            DEBUG_STREAM.println(F("succesfully set flight mode"));
        } else {
            DEBUG_STREAM.println(F("unable to set flight mode!"));
        }
    #endif
    return gps_success;
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
    } else {
    #ifdef DEBUG
        DEBUG_STREAM.println(F("invalid gps location!"));
    #endif
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
