#ifndef SETUP_H
#define SETUP_H

#include "LoRaMac.h"
#include "pico/stdlib.h"
#include "pico/lorawan.h"


typedef struct {
    char* device_eui;
    char app_eui[16];
    char app_key[32];
    char* channel_mask;
    LoRaMacRegion_t region;
    uint8_t adcs_in_use;
} BatteryMonitConfig;


void setup_config(BatteryMonitConfig *bmc);

#endif