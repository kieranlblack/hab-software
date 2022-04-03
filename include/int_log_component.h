#ifndef INT_LOG_COMPONENT_H
#define INT_LOG_COMPONENT_H

#include "log_component.h"

class IntLogComponent : public LogComponent {
private:
    int *data_ptr;

public:
    IntLogComponent(int *data_ptr, const char *name);
    size_t log_data(Print &stream) override;
};

#endif
