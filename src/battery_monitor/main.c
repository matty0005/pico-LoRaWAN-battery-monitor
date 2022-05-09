/**
 * A LoRaWAN based voltage measurement logger using the Raspberry Pi Pico 
 * microcontroller and RFM95 LoRa module. 
 * 
 * @author Matthew Gilpin
 * @date 12 April 2022
*/

#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/lorawan.h"

#include "config.h"
#include "tusb.h"

#include "setup.h"
#include "measurements.h"
#include "rv-3028.h"

#define FEATHER_SPI_SCK 18
#define FEATHER_SPI_MOSI 19
#define FEATHER_SPI_MISO 20
#define FEATHER_SPI_CS 8
#define FEATHER_SPI_INSTANCE spi0
#define FEATHER_RESET 9
#define FEATHER_DIO0 7
#define FEATHER_DIO1 10

#define LATCH_PIN 13

#define BUFFER_SIZE 8
#define RETRY_COUNT 5




extern LoRaMacRegion_t loraRegionOptions[];

// pin configuration for SX1276 radio module
const struct lorawan_sx1276_settings sx1276_settings = {
    .spi = {
        .inst = PICO_DEFAULT_SPI_INSTANCE,
        .mosi = FEATHER_SPI_MOSI,
        .miso = FEATHER_SPI_MISO,
        .sck  = FEATHER_SPI_SCK,
        .nss  = FEATHER_SPI_CS
    },
    .reset = FEATHER_RESET,
    .dio0  = FEATHER_DIO0, // labeled as IRQ on featherwing
    .dio1  = FEATHER_DIO1
};


// variables for receiving data
int receive_length = 0;
uint8_t receive_buffer[242];
uint8_t receive_port = 0;

void latch_power() {
    gpio_init(LATCH_PIN);

    // Set to pull up
    gpio_set_dir(LATCH_PIN, GPIO_OUT);
    
    // Keep power on until we turn it off
    gpio_put(LATCH_PIN, true);
}

void unlatch_power() {
    gpio_init(LATCH_PIN);

    // Set to pull up
    gpio_set_dir(LATCH_PIN, GPIO_OUT);

    // Disconnect power
    gpio_put(LATCH_PIN, false);
}

void hardware_init() {
    // initialize stdio and wait for USB CDC connect
    stdio_init_all();

    for (uint8_t i = 0; i < 5; i++) {

        if (stdio_usb_connected()) {
            break;
        }

        sleep_ms(100);
    }

}

void make_lora_payload(BatteryMonitConfig *bmc, uint8_t *data) {
    
    Measurements meas = {
        .a0 = 0,
        .a1 = 0,
        .a2 = 0,
        .a3 = 0,
    };

    take_measurements(bmc, &meas);

    data[0] = meas.a0 >> 8;
    data[1] = meas.a0 & 0xFF;

    data[2] = meas.a1 >> 8;
    data[3] = meas.a1 & 0xFF;

    data[4] = meas.a2 >> 8;
    data[5] = meas.a2 & 0xFF;

    data[6] = meas.a3 >> 8;
    data[7] = meas.a3 & 0xFF;    

    printf("data: %x %x | %x %x | %x %x | %x %x \n", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
   
}


int main( void ) {

    // first thing we need to do is set power to on.
    // latch_power();

    BatteryMonitConfig conf;
    
    hardware_init();

    setup_config(&conf);

    measurements_init(&conf);

    uint8_t payload[8];
    
    const struct lorawan_otaa_settings otaa_settings = {
        .device_eui   = conf.device_eui,
        .app_eui      = conf.app_eui,
        .app_key      = conf.app_key,
        .channel_mask = LORAWAN_CHANNEL_MASK
    };

    #ifdef DEBUG_MODE

    // uncomment next line to enable debug
    lorawan_debug(true);
    
    #endif

    // initialize the LoRaWAN stack
    printf("Initilizating LoRaWAN ... ");
    
    if (lorawan_init_otaa(&sx1276_settings, loraRegionOptions[conf.region], &otaa_settings) < 0) {
        printf("failed!!!\n");
        while (1) {
            tight_loop_contents();
        }
    } else {
        printf("success!\n");
    }

    // Start the join process and wait
    printf("Joining LoRaWAN network ... ");
    lorawan_join();

    while (!lorawan_is_joined()) {
        lorawan_process();
    }
    printf("joined successfully!\n");

    uint32_t last_message_time = 0;

    #ifdef DEBUG_MODE

    // loop forever
    while (1) {
        

        // let the lorwan library process pending events
        lorawan_process();

        // get the current time and see if 5 seconds have passed
        // since the last message was sent
        uint32_t now = to_ms_since_boot(get_absolute_time());

        if ((now - last_message_time) > 5000) {

            // Record measuremnts and make payload
            make_lora_payload(&conf, payload);

            // try to send an unconfirmed uplink message
            if (lorawan_send_unconfirmed(payload, 8, 2) < 0) {
                printf("failed!!!\n");
            } else {
                printf("success!\n");
            }

            last_message_time = now;
        }
    }

    #else

    // let the lorwan library process pending events
    lorawan_process();

    // Record measuremnts and make payload
    make_lora_payload(&conf, payload);

    // try to send an unconfirmed uplink message
    for (int i = 0; i < RETRY_COUNT && lorawan_send_unconfirmed(payload, 8, 2) < 0; i++) {

        printf("Failed to send: %d\n", i);
        sleep_ms(100);
        lorawan_process();
    }

    lorawan_process_timeout_ms(30000);

    // Turn off device to conserve power
    unlatch_power();

    while (1) {
        tight_loop_contents();
    }

    #endif


    return 0;
}
