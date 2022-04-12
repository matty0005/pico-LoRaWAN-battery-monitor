
#include "measurements.h"

void measurements_init(BatteryMonitConfig *bmc) {

    adc_init();

    // Work out the pins to init
    // ADC pins are from GPIO26 to GPIO39
    for (int i = 0; i < 4; i++) {

        // check if bit is set
        if (!!(bmc->adcs_in_use & (1 << i))) {

            // 26 pin offset.
            adc_gpio_init(i + 26);
        }
    }

    adc_select_input(0);

}

void take_measurements(BatteryMonitConfig *bmc, Measurements *meas) {

    // Normally we'd have a conversion factor here, but since we are 
    // transmitting raw values and computing the voltages on the 
    // reciever side, we'll omit this step here
    // const float conversion_factor = 3.3f / (1 << 12);

    for (int i = 0; i < 4; i++) {

        // check if bit is set
        if (!!(bmc->adcs_in_use & (1 << i))) {

            adc_select_input(i);

            switch (i) {
                case 0:
                    meas->a0 = adc_read();
                    break;
                case 1:
                    meas->a1 = adc_read();
                    break;
                case 2:
                    meas->a2 = adc_read();
                    break;
                case 3:
                    meas->a3 = adc_read();
                    break;
            }


        }
    }
}