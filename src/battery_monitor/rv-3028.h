#ifndef RV_3028_H
#define RV_3028_H

#include "hardware/i2c.h"
#include <stdbool.h>
#include "pico/stdlib.h"

#define SDA_PIN 2
#define SCL_PIN 3
#define I2C_CHANNEL i2c1

void rv3028_init(uint16_t interval);
void set_switchover();
void set_tickle_charge(bool enabled);
void set_24h_time();
void set_time(uint8_t min, uint8_t hour, uint8_t date, uint8_t month, uint16_t year);
uint8_t get_mins();
uint8_t get_hours();
uint8_t get_date();
uint8_t get_month();
uint16_t get_year();
void set_periodic_interrupt(uint16_t period);
void rv3028_clear_interrupts();

#endif