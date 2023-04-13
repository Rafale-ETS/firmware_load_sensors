#ifndef CUSTOM_TYPES_H
#define CUSTOM_TYPES_H 

#include <Arduino.h>
#include <Adafruit_BNO08x.h>

const int AVG_BUF_LEN = 3;
typedef struct LoadValues{
    float avg_buf[AVG_BUF_LEN];
    float raw;
    float kg;
    float newt;
} LoadValue;

typedef struct ImuDatas{
    sh2_Accelerometer_t accel;
    sh2_Gyroscope_t gyro;
    sh2_Accelerometer_t linear;
    sh2_RotationVectorWAcc_t rotation;
} ImuData;

const char STATUS_STARTING[] = "starting";
const char STATUS_ACTIVE[] = "active";
const char STATUS_IDLE[] = "idle";
const char STATUS_LOW_BATT[] = "low_battery";
const char STATUS_NO_IMU[] = "no_imu";
const char STATUS_NO_LOADCELL[] = "no_loadcell";
const char STATUS_NO_TIME[] = "no_time";
const char STATUS_TARE[] = "tare";

const char STATUS_TARE_WAIT_EMPTY[] = "tare_wait_empty";
const char STATUS_TARE_WAIT_WEIGHT[] = "tare_wait_weight";

typedef struct StatusValues {
    char** statuses; // list of status
    unsigned int nb_statuses;
    float batteryVoltage; // battery voltage
    float chargeLevel; // in %
} StatusValue;

#endif