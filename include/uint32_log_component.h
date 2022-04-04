#ifndef UINT32_LOG_COMPONENT_H
#define UINT32_LOG_COMPONENT_H

#include <Arduino.h>

#include "log_component.h"

class UInt32LogComponent : public LogComponent {
private:
    uint32_t *data_ptr;

public:
    UInt32LogComponent(uint32_t *data_ptr, const char *name);
    size_t log_data(Print &stream) override;
};

#endif
