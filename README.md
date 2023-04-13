# Load Sensor Firmware for Rafale's Moth Onboard Data Collection System
This repository contains the firmware code for data collection sensors collecting load forces of the Moth's shrouds and fore stay, as well as orientation (IMU) data.

This code is made for a sensor setup using the following modules:
- Arduino MKR 1010 WiFi
- Most 4 (+shield) wires Load Cell
- HX711 Load Cell Amplifier
- BNO085 IMU
- LiPo 3.7V battery

This sensor is wireless and sends it's data to a MQTT hub/brocker over a WiFi connection. It also uses that same WiFi connection to syncronise it's internal clock/RTC via NPT. Rafale's system uses a Raspberry Pi with Mosquitto as a MQTT Brocker and the ntp linux package setup as a ntp server. This Raspberry Pi is configured as a WiFi access point and every sensor connects to it directly.

This project is developped with PlatformIO via vscode. `pio_tare_test` is the platformIO project folder.

## Calibrating the Load Cell
The load cell requires regular manual calibration. Calibration is handled with a bidirectional MQTT via the `control` and `status` topics.

The `tools/tare.py` script is an exemple of a script of the controller side of this process. It requires paho-mqtt, needs to be modified to not use a custom logger to be directly used and is made to be ran on the same device as the mqtt broker it needs to connect to.

## Connecting to a HUB
To enable the WiFi connection, you need to add a file called `secret.h` in the `pio_tare_test/include` folder.
This file needs to looks like this:
```C
#define HUB_SSID "<your wifi SSID>"
#define HUB_PASS "<your wifi password>"
```

## Building the project
1) Clone this repository
2) Open the `pio_tare_test` folder in VSCode with PlatformIO addon installed.
3) Open platformIO tasks pannel
4) Build, Upload and/or Upload and Monitor as desired.

## Used Libraries:
- ArduinoLowPower -> low power sleep mode
- WIFININA -> MKR 1010 WiFi module's library
- RTCZero -> easy RTC for timestamps
- ArduinoJson -> data formating for MQTT paquets
- XH711 Arduino Library -> reading data from the HX711 load cell amplifier
- Adafruit BNO08x -> reading data from the BNO085 IMU
- FlashStorage -> saving calibration parameters to Flash between resets
- Arduino_BQ24195 -> MKR 1010 PMIC configuration for battery level readings
- PubSubClient -> MQTT communication

*This project is open sourced under the GNU GPL v3, enjoy and share the freedom.*
