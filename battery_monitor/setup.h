#ifndef SETUP_H
#define SETUP_H

#include "LoRaMac.h"


typedef struct {
    const char* device_eui;
    const char app_eui[16];
    const char app_key[32];
    const char* channel_mask;
    LoRaMacRegion_t region;
} BatteryMonitConfig


void setup_config();

#endif