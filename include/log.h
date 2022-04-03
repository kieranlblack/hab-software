#ifndef LOG_H
#define LOG_H

#include "log_component.h"

#define MAX_LOG_COMPONENTS 16

class Log {
private:
    int num_registered;
    LogComponent *log_components[MAX_LOG_COMPONENTS];

public:
    Log();
    bool register_log_component(LogComponent *log_component);
    bool write_log(Print &stream, bool log_header);
};

#endif
