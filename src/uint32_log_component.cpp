#include "uint32_log_component.h"

UInt32LogComponent::UInt32LogComponent(uint32_t *data_ptr, const char *name) : LogComponent(name), data_ptr(data_ptr) {};

size_t UInt32LogComponent::log_data(Print &stream) {
    return stream.print(*data_ptr);
}
