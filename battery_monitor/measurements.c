
#include "measurements.h"

void measurements_init(BatteryMonitConfig *bmc) {

    adc_init();

    // Work out the pins to init
    bmc->adcs_in_use;

}