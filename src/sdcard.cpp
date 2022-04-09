#include "sdcard.h"

#include <SdFat.h>

#include "config.h"

static SdFat sd;
static SdFile active_file;

bool setup_sd() {
    bool success = sd.begin(PIN_SD_CS, SPI_HALF_SPEED);
    #if DEBUG_SD
        if (success) {
            DEBUG_STREAM.println(F("sd card working"));
        } else {
            sd.initErrorPrint();
        }
    #endif
    return success;
}

bool log_to_sd(Log &log, bool log_header) {
    bool success = active_file.open(SD_LOG_FILE, O_RDWR | O_CREAT | O_AT_END);
    if (success) {
        log.write_log(active_file, log_header);
        active_file.close();
    }
    #if DEBUG_SD
        if (!success) {
            DEBUG_STREAM.println(F("failed to write to sd"));
        }
    #endif
    return success;
}
