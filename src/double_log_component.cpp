#include "double_log_component.h"

DoubleLogComponent::DoubleLogComponent(double *data_ptr, const char *name) : LogComponent(name), data_ptr(data_ptr) {};

size_t DoubleLogComponent::log_data(Print &stream) {
    return stream.print(*data_ptr, 7);
}
