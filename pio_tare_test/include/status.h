#ifndef STATUS_H
#define STATUS_H 

#include <BQ24195.h>
#include <Arduino.h>

// Information about the battery
const int R1 =  330000;  // resistor between battery terminal and SAMD pin PB09
const int R2 = 1000000; // resistor between SAMD pin PB09 and ground
const float BATTERY_FULL_VOLTAGE = 4.2; // upper voltage limit for battery in V
const float BATTERY_EMPTY_VOLTAGE = 3.5;// lower voltage limit for battery in V
float BATTERY_CAPACITY = 1.5; // battery capacity in Ah

typedef struct StatusValues {
    // TODO : Add status
    float batteryVoltage; // battery voltage
    float chargeLevel; // in %
} StatusValue;

void setup_status();
void loop_status();

void read_status_no_block(StatusValue* container);

#endif