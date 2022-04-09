#include "voltage.h"

#include "config.h"

uint32_t voltage;

bool read_voltage() {
    analogReference(DEFAULT);
    analogRead(PIN_VOLTMETER);
    delay(10);
    int adc = analogRead(PIN_VOLTMETER);
    voltage = (uint32_t) (5000. * adc / 1024.) * (VOLTMETER_R1 + VOLTMETER_R2) / VOLTMETER_R2;
    return true;
}
