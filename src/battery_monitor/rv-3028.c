#include "rv-3028.h"

#define STATUS_REG 0x0E
#define CONTROL_1_REG 0x0F
#define CONTROL_2_REG 0x10
#define TIMER_VALUE_0_REG 0x0A
#define TIMER_VALUE_1_REG 0x0B

#define I2C_ADDRESS 0x52

void set_periodic_interrupt(uint16_t period);


uint8_t decimal_to_bcd(uint8_t value) {
    return ((value / 10) * 0x10) + (value % 10);

}

uint8_t bcd_to_decimal(uint8_t value) {
    return ((value / 0x10) * 10) + (value % 0x10);
}

void rv3028_write(uint8_t reg, uint8_t value) {

    uint8_t buff[2] = { reg, value };

    // Wait 100ms
    i2c_write_timeout_us(I2C_CHANNEL, I2C_ADDRESS, buff, 2, true, 1000 * 100);
}

uint8_t rv3028_read(uint8_t reg) {

    i2c_write_timeout_us(I2C_CHANNEL, I2C_ADDRESS, &reg, 1, true, 1000 * 100);

    char buff[1];
    i2c_read_timeout_us(I2C_CHANNEL, I2C_ADDRESS, buff, 1, true, 1000 * 100);

    return buff[0];

}


void rv3028_init(uint16_t interval) {
    // Use SCL = GPIO3
    // Use SDA = GPIO2

    // Initalise i2c at 100khz
    i2c_init(I2C_CHANNEL, 100000);

    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);

    // Pull ups are on the RTC so no need to pull up    


    set_switchover();

    set_tickle_charge(true);
    
    // Set to 24h time
    set_24h_time();

    // Set interval time
    set_periodic_interrupt(interval);

}

void clear_bits(uint8_t reg, uint8_t bitmask) {
    uint8_t current_value = rv3028_read(reg);

    current_value &= ~bitmask;

    rv3028_write(reg, current_value);
}

void set_bits(uint8_t reg, uint8_t bitmask) {
    uint8_t current_value = rv3028_read(reg);

    current_value &= ~bitmask;
    current_value |= bitmask;

    rv3028_write(reg, current_value);
    
}

void set_periodic_interrupt(uint16_t period) {

    // Clear TE
    clear_bits(CONTROL_1_REG, ((1 << 2) | 0x03));

    //Clear TIE
    clear_bits(CONTROL_2_REG, (1 << 4));

    //Clear TF
    rv3028_clear_interrupts();

    // Set TRPT bit to reload interrupt and set timer clock to 1/60hz
    set_bits(CONTROL_1_REG, ((1 << 7) | 0x03)); //0x03 for 1/60, 0x02 for 1hz

    // Work out period
    // Timer Value 0 is low reg.
    // Clear bits first
    clear_bits(TIMER_VALUE_0_REG, 0xFF);
    clear_bits(TIMER_VALUE_0_REG, 0xFF);

    set_bits(TIMER_VALUE_0_REG, period & 0xFF);
    set_bits(TIMER_VALUE_1_REG, (period >> 8) & 0xFF);

    // Enable hardware interrupt pin.
    set_bits(CONTROL_2_REG, (1 << 4));

    // Enable countdown timer.
    set_bits(CONTROL_1_REG, (1 << 2));

}

void rv3028_clear_interrupts() {
    //Clear TF
    clear_bits(STATUS_REG, (1 << 3));
}


void set_24h_time() {
    
    clear_bits(0x10, (1 << 1));
}

void set_switchover() {
    // Set switchover mode to switch when Vd < Vbackup
    uint8_t eeprom_backup_reg = rv3028_read(0x37);

    eeprom_backup_reg &= ~(0x03 << 2);
    eeprom_backup_reg |= (0x01 << 2);

    rv3028_write(0x37, eeprom_backup_reg);

}

void set_tickle_charge(bool enabled) {
    // EEPROM backup 37h -> TCE = 1
    // Use default 3k resistor
    
    uint8_t eeprom_backup_reg = rv3028_read(0x37);

    // TCE is bit 5, and reset TCR to 0.
    eeprom_backup_reg &= ~((1 << 5) | 0x03);
    eeprom_backup_reg |= (!!(enabled) << 5);

    rv3028_write(0x37, eeprom_backup_reg);

}

void set_time(uint8_t min, uint8_t hour, uint8_t date, uint8_t month, uint16_t year) {
    // Set 0 seconds
    rv3028_write(0x00, 0);

    // Set mins
    rv3028_write(0x01, decimal_to_bcd(min));

    // Set hours
    rv3028_write(0x02, decimal_to_bcd(hour));

    // Set date
    rv3028_write(0x04, decimal_to_bcd(date));

    // Set month
    rv3028_write(0x05, decimal_to_bcd(month));

    // Set year, in format 00 - 99
    year -= 2000;
    rv3028_write(0x06, decimal_to_bcd(year));

}

uint8_t get_mins() {
    return bcd_to_decimal(rv3028_read(0x01));
}

uint8_t get_hours() {
    return bcd_to_decimal(rv3028_read(0x02));
}
uint8_t get_date() {
    return bcd_to_decimal(rv3028_read(0x04));
}
uint8_t get_month() {
    return bcd_to_decimal(rv3028_read(0x05));
}
uint16_t get_year() {
    return bcd_to_decimal(rv3028_read(0x06)) + 2000;
}
