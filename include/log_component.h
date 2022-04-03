#ifndef LOG_COMPONENT_H
#define LOG_COMPONENT_H

#include <Arduino.h>

class LogComponent {
private:
    const char *name;

public:
    LogComponent(const char *name);
    virtual size_t log_data(Print &stream) = 0;
    const char* get_name();
};

#endif
