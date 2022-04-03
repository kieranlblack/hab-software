#ifndef DOUBLE_LOG_COMPONENT_H
#define DOUBLE_LOG_COMPONENT_H

#include "log_component.h"

class DoubleLogComponent : public LogComponent {
private:
    double *data_ptr;

public:
    DoubleLogComponent(double *data_ptr, const char *name);
    size_t log_data(Print &stream) override;
};

#endif
