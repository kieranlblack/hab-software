#include "mprls.h"

#include <Adafruit_MPRLS.h>

Adafruit_MPRLS mprls = Adafruit_MPRLS(-1, -1);

double pressure = 0;

bool setup_mprls() {
    int attempts_remaining = 3;
    bool success = false;
    while (!success && attempts_remaining > 0) {
        success = mprls.begin();
        if (!success) {
            #ifdef DEBUG
                DEBUG_STREAM.print(F("unable to setup mprls, "));
                DEBUG_STREAM.print(attempts_remaining - 1);
                DEBUG_STREAM.println(F(" attempts remaining"));
            #endif
            delay(500);
        } else {
            #ifdef DEBUG
                DEBUG_STREAM.println(F("mprls setup correctly"));
            #endif
        }
        attempts_remaining--;
    }
    return success;
}

bool read_mprls() {
    pressure = (double) mprls.readPressure();
    return true;
}
