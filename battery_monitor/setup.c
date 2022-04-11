#include <stdbool.h>

bool is_in_config_mode() {

    // Check if GPIO pin is tied to ground


    return true;
}

void setup_config() {
    
    // Check if in setup mode
    if (!is_in_config_mode()) {
        return;
    }

    // Read info like device eui etc
    char devEui[17];

    lorawan_default_dev_eui(devEui);



}