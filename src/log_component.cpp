#include "log_component.h"

LogComponent::LogComponent(const char *name) : name(name) {};

const char *LogComponent::get_name() {
    return name;
}
