#ifndef IMU_H
#define IMU_H

#include <Arduino.h>
#include <Adafruit_BNO08x.h>
#include <ArduinoLowPower.h>
#include "custom_types.h"

const uint8_t BNO_ADDRESS = 0x4b;

void setReports(Adafruit_BNO08x* bno08x);
void setup_imu(Adafruit_BNO08x* bno08x);

sh2_SensorId_t read_imu_no_block(Adafruit_BNO08x* bno08x, ImuData* container);

#endif