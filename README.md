# pico-LoRaWAN-battery-monitor
A raspberry pi pico based lorawan enabled battery monitor

## DIYODE Magazine article writeup
This is the codebase for the article I wrote in issue 61.

[Part 1 - Issue 61](https://diyodemag.com/projects/raspberry_pi_feather_rp2040_lorawan_battery_monitor_part_1)

[Part 2 - Issue 62](https://diyodemag.com/projects/raspberry_pi_feather_rp2040_lorawan_battery_monitor_part_2)



# Device


## Schematic 
<img alt="image" src="https://cdn.mattgilpin.au/images/github/lorawan_battery_monitor_schematic.png">

### Default Pinout for RFM95

| Pi Pico (RP2040) | RFM95 (Semtech SX1276) |
| ----------------- | -------------- |
| 3.3V | VCC |
| GND | GND |
| GPIO 20 | MISO |
| GPIO 19 | MOSI |
| GPIO 18 | SCK |
| GPIO 8 | CS |
| GPIO 7 | DIO0  |
| GPIO 10 | DIO1 |
| GPIO 9 | RESET |


### Default Pinout for RV-3028 RTC

| Pi Pico (RP2040) | RV-3028 |
| ----------------- | -------------- |
| 3.3V | VCC |
| GND | GND |
| GPIO 3 | SCL |
| GPIO 2 | SDA |


### Default Pinout for measurements

| Pi Pico (RP2040) | Connection |
| ----------------- | -------------- |
| GPIO 13 | Power latch |
| GPIO 11 | Config Pin |
| GPIO 26 (A0) | Voltage 1 |
| GPIO 27 (A1) | Voltage 2 |
| GPIO 28 (A2) | Voltage 3 |
| GPIO 29 (A3) | Voltage 4 |
| GPIO 6  | Measure enable |

## Software
Once you have installed the latest version of docker, run the following command in a suitable directory
```
git clone https://github.com/matty0005/lorawan_battey_monitor_software
cd lorawan_battey_monitor_software
docker-compose up -d
```
More instructions at: https://github.com/matty0005/lorawan_battey_monitor_software

## Cloning 
```
git clone --recursive-submodules https://github.com/matty0005/pico-LoRaWAN-battery-monitor
```

## Compiling
You will need to have the Raspberry Pi Pico C/C++ SDK installed on your machine. 

Once installed, you can build the code by running the following commands
```
mkdir build
cd build
cmake .. -DPICO_BOARD=pico
make -j
```
Then you can find the .uf2 file in the build folder and drag it over to your pi pico.

### Notes
Based off the pico-lorawan library: https://github.com/ArmDeveloperEcosystem/lorawan-library-for-pico

