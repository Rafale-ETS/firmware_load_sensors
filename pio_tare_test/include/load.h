#ifndef LOAD_H
#define LOAD_H 

#include <Arduino.h>
#include <HX711.h>
#include <ArduinoLowPower.h>
#include <FlashStorage.h>
#include <Adafruit_BNO08x.h>

/*
Load cell specs:
Output:                 2 mv/V => 5V: 10mV, 3V3: 6.6mV
Non-lin:                0.019 %FS => n/a
Hysteresis:             0.018 %FS => n/a
Repeatability:          0.020 %FS => n/a
Creep:                  0.015 %FS/30mins => n/a
Temp effect:            0.017 %FS/10C => n/a
Zero balance:           +/0- 1 %FS => n/a
Load RES:               350+/-5 Ohms.
Insul RES:              >5000 MOhm @ 50VDC
Rec exitation V.:       9-12 V
Allowed exitation V:    5-18 V
Compensated temp:       -10C -> +40C
Op temp:                -20C -> 55C
Overload:               1.2x (120 %FS)[ultimate: 150 %FS]
*/

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 5;
const int LOADCELL_SCK_PIN = 4;

const int AVG_BUF_LEN = 5;
typedef struct LoadValues{
    float avg_buf[AVG_BUF_LEN];
    float raw;
    float kg;
    float newt;
} LoadValue;

const float kg_to_newts = 9.80665; // = 1 kgf
//calibration factor will be the (reading)/(known weight)

void tare();

void setup_loadcell();
void loop_loadcell();

bool read_cell_no_block(LoadValue* container);

#endif