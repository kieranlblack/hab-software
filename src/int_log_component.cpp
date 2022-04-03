#include "int_log_component.h"

IntLogComponent::IntLogComponent(int *data_ptr, const char *name) : LogComponent(name), data_ptr(data_ptr) {};

size_t IntLogComponent::log_data(Print &stream) {
    return stream.print(*data_ptr);
}
