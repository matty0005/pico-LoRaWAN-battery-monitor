
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

uint16_t avg_adc_read(int n) {
    uint64_t value = 0;

    for (int i = 0; i < n; i++) {
        value += adc_read();

        sleep_ms(5);
    }
    
    return (uint16_t)(value / n);
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
                    meas->a0 = avg_adc_read(5);
                    break;
                case 1:
                    meas->a1 = avg_adc_read(5);
                    break;
                case 2:
                    meas->a2 = avg_adc_read(5);
                    break;
                case 3:
                    meas->a3 = avg_adc_read(5);
                    break;
            }


        }
    }
}

