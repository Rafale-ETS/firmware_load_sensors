#ifndef IMU_H
#define IMU_H

#include <Arduino.h>
#include <Adafruit_BNO08x.h>
#include <ArduinoLowPower.h>

const uint8_t BNO_ADDRESS = 0x4b;

typedef struct ImuDatas{
    sh2_Accelerometer_t accel;
    sh2_Gyroscope_t gyro;
    sh2_Accelerometer_t linear;
    sh2_RotationVectorWAcc_t rotation;
} ImuData;

void setReports(Adafruit_BNO08x* bno08x);
void setup_imu(Adafruit_BNO08x* bno08x);

sh2_SensorId_t read_imu_no_block(Adafruit_BNO08x* bno08x, ImuData* container);

#endif