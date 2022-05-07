#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include "setup.h"
#include "hardware/flash.h"
#include "hardware/sync.h"

#include "rv-3028.h"

// We're going to erase and reprogram a region 512k from the start of flash.
// Once done, we can access this at XIP_BASE + 512k.
#define FLASH_TARGET_OFFSET (512 * 1024)

#define CONFIG_PIN 11

const uint8_t *flash_target_contents = (const uint8_t *) (XIP_BASE + FLASH_TARGET_OFFSET);

LoRaMacRegion_t loraRegionOptions[] = {
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

char get_keypress() {
    char option = getchar();
    printf("%c",option);

    // char c;
    // do {
    //     c = getchar();
    // } while (c != '\r' &&  c != '\n' && c != EOF);

    printf("\n");
    return option;
}

void get_string(char *buff, int length) {

    char c;
    int i = 0;

    do {
        c = getchar();
        printf("%c",c);
        buff[i++] = c;

        if (i > length) {
            break;
        }
    } while (c != '\r' &&  c != '\n' && c != EOF);

    // buff[i] = '\0';

    printf("\n");
}



/**
 * Checks if pin CONFIG_PIN (config pin) has been pulled down to enter 
 * config mode.
*/
bool is_in_config_mode() {
    
    gpio_init(CONFIG_PIN);

    // Set to pull up
    gpio_set_dir(CONFIG_PIN, GPIO_IN);
    gpio_pull_up(CONFIG_PIN);

    sleep_ms(100);

    return !gpio_get(CONFIG_PIN);

}

void configure_region(BatteryMonitConfig *bmc) {
    
    printf("\nPlease select your region:\n" \
    "    [0] AS923\n" \
    "    [1] AU915 \n" \
    "    [2] CN470\n" \
    "    [3] EU433\n" \
    "    [4] EU868\n" \
    "    [5] KR920\n" \
    "    [6] IN865\n" \
    "    [7] US915\n" \
    "    [8] RU864\n" \
    "Please enter a selection [0:8] : "
    );


    int option = -1;

    // Make sure value is between 0 and 8
    while (option < 0 || option > 8) {
        char press =  get_keypress();

        if ((int)press < 48 || press > 57) {
            printf("\nNot a valid number\nPlease enter a selection: ");
            continue;
        }
        
        option = (int)press - 48;

    }

    bmc->region = loraRegionOptions[option];

}

void configure_string(char *property, char *store, int buffLen) {
    
    bool isCorrect = false;
    
    while (!isCorrect) {
        printf("%s:", property);
        
        get_string(store, buffLen);

        printf("\nYou have entered in: %.*s\nIs this correct ? [y/n]:", buffLen, store);
        char option = getchar();

        if (option == 'Y' || option == 'y') {
            isCorrect = true;
        } 
        
        printf("\n");
    }       

}

void configure_adc(BatteryMonitConfig *bmc) {

    // Set all bits to 0.
    bmc->adcs_in_use = 0;
        
    for (int i = 0; i < 4; i++) {
        printf("Do you want to enable measurement on pin A%d [y/n]: ", i);

        char option = getchar();

        // Add a 1 in a bit position to enable
        if (option == 'Y' || option == 'y') {
            bmc->adcs_in_use |= (1 << i);
        } 
        
        printf("\n");
    }       

}


void configure_app_eui(BatteryMonitConfig *bmc) {

    configure_string("Please enter your APP EUI here", bmc->app_eui, 16);    
}

void configure_app_key(BatteryMonitConfig *bmc) {

    configure_string("Please enter your APP KEY here", bmc->app_key, 32);
}

void configure_frequency(BatteryMonitConfig *bmc) {
    char period[4];

    configure_string("Please enter your measurment period", period, 4);

    char *end;
    // Convert string to uint16_t
    bmc->measure_period = strtol(period, &end, 10);
    printf("Setting time to be %d mins\n", bmc->measure_period);

    set_periodic_interrupt(bmc->measure_period);
}

void configure_datetime() {
    char year[4], month[2], day[2], hour[2], mins[2];

    configure_string("The curent year is", year, 4);
    configure_string("The curent month is (1-12)", month, 2);
    configure_string("The curent date is (1-31)", day, 2);
    configure_string("The curent hour is (0-23)", hour, 2);
    configure_string("The curent minute is (0-59)", mins, 2);

    char *end;
    // Convert string to uint16_t
    uint16_t yearVal = strtol(year, &end, 10);
    uint8_t monthVal = strtol(month, &end, 10);
    uint8_t dayVal = strtol(day, &end, 10);
    uint8_t hourVal = strtol(hour, &end, 10);
    uint8_t minsVal = strtol(mins, &end, 10);

    set_time(minsVal, hourVal, dayVal, monthVal, yearVal);
    printf("The time has been set\n");

}

void restore_config_from_flash(BatteryMonitConfig *bmc) {
    
    if (flash_target_contents[0] != 0xF0) {

        printf("No current configuration has been saved: %x\n", flash_target_contents[0]);
        return;
    }

    bmc->adcs_in_use = flash_target_contents[1];
    bmc->measure_period = (flash_target_contents[2] << 8) || flash_target_contents[3];
    bmc->region = flash_target_contents[4];


    memcpy(bmc->app_eui, (flash_target_contents + 5), 16);
    memcpy(bmc->app_key, (flash_target_contents + 23), 32);

    printf("Saved app eui: %.16s\n", bmc->app_eui);
    printf("Saved app key: %.32s\n",bmc->app_key);
    printf("Saved region: %d\n",bmc->region);
    printf("Saved measurement period: %d\n",bmc->measure_period);
}

/**
 * Writes the config to flash in specified format
 * 
 * Fomrat: 
 *  | 0xF0 | adcs_in_use x1 | measure_period x2 | region x1 | app_eui x16 | app_key x32 | 
 * 
 * Page size in flash = 128 bytes
 * 
*/
void flash_write_config(BatteryMonitConfig *bmc) {

    // Prepare data
    uint8_t data_to_store[FLASH_PAGE_SIZE] = {0};

    // Add in region bytes
    data_to_store[0] = 0xF0;
    data_to_store[1] = bmc->adcs_in_use;
    data_to_store[2] = (bmc->measure_period >> 8) & 0xFF;
    data_to_store[3] = bmc->measure_period & 0xFF;
    data_to_store[4] = bmc->region;

    // Copy over the app_eui and app_key
    memcpy((data_to_store + 5), bmc->app_eui, 16);
    memcpy((data_to_store + 23), bmc->app_key, 32);



    // Note that a whole number of sectors must be erased at a time.
    printf("\nErasing target region...\n");

    // Have to disable interrrupts
    uint32_t ints = save_and_disable_interrupts();
    flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);

    //Renable
    restore_interrupts(ints);

    printf("\nSaving config to flash...\n");

    ints = save_and_disable_interrupts();
    flash_range_program(FLASH_TARGET_OFFSET, data_to_store, FLASH_PAGE_SIZE);
    restore_interrupts(ints);

    printf("Done. Verifying...\n");

    bool mismatch = false;
    for (int i = 0; i < FLASH_PAGE_SIZE; ++i) {
        if (data_to_store[i] != flash_target_contents[i])
            mismatch = true;
    }
    if (mismatch)
        printf("Saving to flash failed!\n");
    else
        printf("Saved to flash successful!\n");
}


void setup_config(BatteryMonitConfig *bmc) {

    // Bring settings out of flash if any
    restore_config_from_flash(bmc);
    
    // Check if in setup mode
    if (!is_in_config_mode()) {
        return;
    }

    printf("Welcome to the setup mode.\n");


    // Read info like device eui etc
    char devEui[16];
    lorawan_default_dev_eui(devEui);
    printf("Device EUI: %s\n", devEui);

    // copy dev_eui to struct
    memcpy(bmc->device_eui, devEui, 16);

    bool configure = true;

    while (configure) {
        printf("What would you like to configure\n" \
        "    [0] Region\n" \
        "    [1] APP Eui,\n" \
        "    [2] APP Key\n" \
        "    [3] Pin's to measure\n" \
        "    [4] Frequency of measurement\n" \
        "    [5] Set date and time\n" \
        "Press q to quit, or enter selection:");

        char option = get_keypress();


        switch (option) {
            case '0':
                configure_region(bmc);
                break;
            case '1':
                configure_app_eui(bmc);
                break;
            case '2':
                configure_app_key(bmc);
                break;
            case '3':
                configure_adc(bmc);
                break;
            case '4':
                configure_frequency(bmc);
                break;
            case '5':
                configure_datetime();
                break;
            case 'c':
                rv3028_clear_interrupts();
                break;
            case 'q':
            default:
                configure = false;
                break;
        }
    }


    // Save config to flash
    flash_write_config(bmc); 
    printf("\nYour settings have been saved. Please remove the config wire and restart device.\n");
    


}