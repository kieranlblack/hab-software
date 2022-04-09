#include "log.h"

#include "config.h"

Log::Log(): num_registered(0) {};

bool Log::register_log_component(LogComponent *log_component) {
    #ifdef DEBUG_LOG
        DEBUG_STREAM.print(F("registering to log: "));
        DEBUG_STREAM.println(log_component->get_name());
    #endif
    if (num_registered >= MAX_LOG_COMPONENTS) {
        #ifdef DEBUG_LOG
            DEBUG_STREAM.println(F("log components full"));
        #endif
        return false;
    }
    log_components[num_registered] = log_component;
    num_registered++;
    return true;
}

bool Log::write_log(Print &stream, bool log_header) {
    for (int i = 0; i < num_registered; i++) {
        if (log_header) {
            stream.print(log_components[i]->get_name());
        } else {
            log_components[i]->log_data(stream);
        }
        if (i < num_registered - 1) {
            stream.print(DELIMITER);
        }
    }
    stream.println();
    return true;
}
