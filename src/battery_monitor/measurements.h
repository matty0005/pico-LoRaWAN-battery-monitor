#ifndef MEASUREMENTS_H
#define MEASUREMENTS_H

#include "hardware/adc.h"
#include "setup.h"

typedef struct {
    uint16_t a0;
    uint16_t a1;
    uint16_t a2;
    uint16_t a3;
} Measurements;

void measurements_init(BatteryMonitConfig *bmc);
void take_measurements(BatteryMonitConfig *bmc, Measurements *meas);

#endif