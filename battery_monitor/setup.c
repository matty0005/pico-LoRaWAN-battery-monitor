#include <stdbool.h>
#include <stdio.h>

#include "setup.h"

bool is_in_config_mode() {

    // Check if GPIO pin is tied to ground


    return true;
}

void configure_region(BatteryMonitConfig *bmc) {
    //
}

void configure_app_eui(BatteryMonitConfig *bmc) {
    //
}
void configure_app_key(BatteryMonitConfig *bmc) {
    //
}

void setup_config(BatteryMonitConfig *bmc) {
    
    // Check if in setup mode
    if (!is_in_config_mode()) {
        return;
    }

    // Read info like device eui etc
    char devEui[17];

    lorawan_default_dev_eui(devEui);

    printf("Welcome to the setup mode.\r\n");

    printf("Device EUI: %s\r\n", devEui);

    bool configure = true;

    while (configure) {
        printf("What would you like to configure\r\n    [0] Region\r\n    [1] APP Eui,\r\n    [2] APP Key\r\nPress q to quit, or enter selection:");

        char option = getchar();

        switch (option) {
            case '0':
                configure_region(bnc);
                break;
            case '1':
                configure_app_eui(bmc);
                break;
            case '2':
                configure_app_key(bmc);
                break;
            case 'q':
                configure = false;
                break;
        }
    }

    

    


}