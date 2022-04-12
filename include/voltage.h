#ifndef VOLTAGE_H
#define VOLTAGE_H

#define VOLTMETER_R1 10000
#define VOLTMETER_R2 2200

#include <Arduino.h>

extern uint32_t voltage;

bool read_voltage();

#endif
