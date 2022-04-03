#ifndef SDCARD_H
#define SDCARD_H

#include "log.h"

bool setup_sd();
bool log_to_sd(Log &log, bool log_header);

#endif
