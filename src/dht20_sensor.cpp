#include "dht20_sensor.h"

#include "config.h"

#include <DHT20.h>
#include <Wire.h>

DHT20 DHT(&Wire);

double humidity = 0;
double temp_dht = 0;

bool setup_dht20() {
    int attempts_remaining = 3;
    bool success = false;
    while (!success && attempts_remaining > 0) {
        success = DHT.begin();
        if (!success) {
            #ifdef DEBUG_DH20
                // copied from the dht20 library example
                DEBUG_STREAM.print(F("unable to setup dht20, issue is "));
                int status = DHT.read();
                switch (status) {
                case DHT20_OK:
                    DEBUG_STREAM.println(F("no issue?"));
                    break;
                case DHT20_ERROR_CHECKSUM:
                    DEBUG_STREAM.print(F("checksum"));
                    break;
                case DHT20_ERROR_CONNECT:
                    DEBUG_STREAM.print(F("connection"));
                    break;
                case DHT20_MISSING_BYTES:
                    DEBUG_STREAM.print(F("bytes missing"));
                    break;
                default:
                    DEBUG_STREAM.print(F("unknown"));
                    break;
                }
                DEBUG_STREAM.print(F(" with "));
                DEBUG_STREAM.print(attempts_remaining - 1);
                DEBUG_STREAM.println(F(" attempts remaining"));
            #endif
            delay(500);
        } else {
            #ifdef DEBUG_DH20
                DEBUG_STREAM.println(F("dht20 set up correctly"));
            #endif
        }
        attempts_remaining--;
    }
    return success;
}

bool read_dht20() {
    humidity = (double) DHT.getHumidity();
    temp_dht = (double) DHT.getTemperature();
    return true;
}
