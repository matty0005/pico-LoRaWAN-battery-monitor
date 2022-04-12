#include <stdbool.h>
#include <stdio.h>

#include "setup.h"

bool is_in_config_mode() {

    // Check if GPIO pin is tied to ground


    return true;
}

void configure_region(BatteryMonitConfig *bmc) {
    
    printf("\r\nPlease select your region:\r\n" \
    "    [0] AS923\r\n" \
    "    [1] AU915 \r\n" \
    "    [2] CN470\r\n" \
    "    [3] EU433\r\n" \
    "    [4] EU868\r\n" \
    "    [5] KR920\r\n" \
    "    [6] IN865\r\n" \
    "    [7] US915\r\n" \
    "    [8] RU864\r\n" \
    "Please enter a selection [0:8] :"
    );


    LoRaMacRegion_t options[] = {
        LORAMAC_REGION_AS923,
        LORAMAC_REGION_AU915,
        LORAMAC_REGION_CN470,
        LORAMAC_REGION_CN779,
        LORAMAC_REGION_EU433,
        LORAMAC_REGION_EU868,
        LORAMAC_REGION_KR920,
        LORAMAC_REGION_IN865,
        LORAMAC_REGION_US915,
        LORAMAC_REGION_RU864,
    };

    int option = -1;

    // Make sure value is between 0 and 8
    while (option < 0 || option > 8) {

        scanf("%d", &option);
    }

    bmc->region = options[option];

}

void configure_string(char *property, char *store) {
    
    bool isCorrect = false;
    
    while (!isCorrect) {
        printf("Please enter your %s here: ", property);
            
        scanf("%d", store);

        printf("\r\nYou have entered in '%s'\r\n", store);
        printf("Is this correct ? [y/n]: ");
        char option = getchar();

        if (option == 'Y' || option == 'y') {
            isCorrect = true;
        }
        
        printf("\r\n");
    }       

}

void configure_app_eui(BatteryMonitConfig *bmc) {

    configure_string("APP EUI", bmc->app_eui);    
}

void configure_app_key(BatteryMonitConfig *bmc) {

    configure_string("APP KEY", bmc->app_key);
}

void restore_config_from_flash(BatteryMonitConfig *bmc) {
    // TODO: later.
    
}

void setup_config(BatteryMonitConfig *bmc) {

    // Bring settings out of flash if any
    restore_config_from_flash(bmc);
    
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
        printf("What would you like to configure\r\n" \
        "    [0] Region\r\n" \
        "    [1] APP Eui,\r\n" \
        "    [2] APP Key\r\n" \
        "Press q to quit, or enter selection:");

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

    // Save config to flash

    

    


}