cmake_minimum_required(VERSION 3.12)

if (TARGET tinyusb_device)
    # rest of your project
    add_executable(pico_lora_battery_monitor
        main.c
        setup.c
        measurements.c
        rv-3028.c
    )

    target_link_libraries(pico_lora_battery_monitor pico_lorawan pico_stdlib hardware_flash hardware_sync hardware_adc hardware_i2c)

    # enable usb output, disable uart output
    pico_enable_stdio_usb(pico_lora_battery_monitor 1)
    pico_enable_stdio_uart(pico_lora_battery_monitor 0)

    # create map/bin/hex/uf2 file in addition to ELF.
    pico_add_extra_outputs(pico_lora_battery_monitor)
elseif(PICO_ON_DEVICE)
    message("TinyUSB submodule is not initalised in the sdk")
endif()