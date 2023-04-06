#ifndef BATTERY_H
#define BATTERY_H 

#include <BQ24195.h>
#include <Arduino.h>

const int R1 =  330000;  // resistor between battery terminal and SAMD pin PB09
const int R2 = 1000000; // resistor between SAMD pin PB09 and ground

const float BATTERY_FULL_VOLTAGE = 4.2; // upper voltage limit for battery in V
const float BATTERY_EMPTY_VOLTAGE = 3.5;// lower voltage limit for battery in V

float BATTERY_CAPACITY = 1.5; // battery capacity in Ah

typedef struct BatteryValues {
    float rawADC; 
    float voltADC;
    float voltBat; // battery voltage
    float chargeLevel; // in %
} BatteryValue;

void setup_battery();
void loop_battery();

void read_battery_no_block(BatteryValue* container);

#endif