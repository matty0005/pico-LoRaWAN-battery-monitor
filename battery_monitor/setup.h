#ifndef SETUP_H
#define SETUP_H

typedef struct {
    const char* device_eui;
    const char* app_eui;
    const char* app_key;
    const char* channel_mask;
} BatteryMonitConfig


void setup_config();

#endif