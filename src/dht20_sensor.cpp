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
                DEBUG_STREAM.print(F("unable to setup dht20, "));
                DEBUG_STREAM.print(attempts_remaining - 1);
                DEBUG_STREAM.println(F(" attempts remaining"));
            #endif
            delay(500);
        } else {
            #ifdef DEBUG_DH20
                DEBUG_STREAM.print(F("dht20 set up correctly"));
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
