#ifndef STATUS_H
#define STATUS_H 

// Include the library for the BQ24195 IC
#include <BQ24195.h>
#include <Arduino.h>
#include "custom_types.h"

// Information about the battery
const int R1 =  330000;  // resistor between battery terminal and SAMD pin PB09
const int R2 = 1000000; // resistor between SAMD pin PB09 and ground
const float BATTERY_FULL_VOLTAGE = 3.8; // upper voltage limit for battery in V
const float BATTERY_EMPTY_VOLTAGE = 3.2;// lower voltage limit for battery in V
const float BATTERY_CAPACITY = 1.5; // battery capacity in Ah

void add_status(StatusValue* container, const char* status);
void rm_status(StatusValue* container, const char* status);
void ch_status(StatusValue* container, int id, const char* new_status);

void setup_status();
void loop_status();

void read_status_no_block(StatusValue* container);

#endif