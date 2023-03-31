#include "imu.h"

// Here is where you define the sensor outputs you want to receive
void setReports(Adafruit_BNO08x* bno08x) {
  Serial.println("Setting desired reports");
  if (!bno08x->enableReport(SH2_ACCELEROMETER)) {
    Serial.println("Could not enable accelerometer");
  }
  if (!bno08x->enableReport(SH2_GYROSCOPE_CALIBRATED)) {
    Serial.println("Could not enable gyroscope");
  }
  if (!bno08x->enableReport(SH2_LINEAR_ACCELERATION)) {
    Serial.println("Could not enable linear acceleration");
  }
  if (!bno08x->enableReport(SH2_ROTATION_VECTOR)) {
    Serial.println("Could not enable rotation vector");
  }
  /*
  if (!bno08x->enableReport(SH2_GRAVITY)) {
    Serial.println("Could not enable gravity vector");
  }
  //if (!bno08x->enableReport(SH2_MAGNETIC_FIELD_CALIBRATED)) {
  //  Serial.println("Could not enable magnetic field calibrated");
  //}
  //if (!bno08x.enableReport(SH2_GEOMAGNETIC_ROTATION_VECTOR)) {
  //  Serial.println("Could not enable geomagnetic rotation vector");
  //}
  //if (!bno08x.enableReport(SH2_GAME_ROTATION_VECTOR)) {
  //  Serial.println("Could not enable game rotation vector");
  //}
  //if (!bno08x.enableReport(SH2_STEP_COUNTER)) {
  //  Serial.println("Could not enable step counter");
  //}
  //if (!bno08x.enableReport(SH2_STABILITY_CLASSIFIER)) {
  //  Serial.println("Could not enable stability classifier");
  //}
  //if (!bno08x.enableReport(SH2_RAW_ACCELEROMETER)) {
  //  Serial.println("Could not enable raw accelerometer");
  //}
  //if (!bno08x.enableReport(SH2_RAW_GYROSCOPE)) {
  //  Serial.println("Could not enable raw gyroscope");
  //}
  //if (!bno08x.enableReport(SH2_RAW_MAGNETOMETER)) {
  //  Serial.println("Could not enable raw magnetometer");
  //}
  //if (!bno08x.enableReport(SH2_SHAKE_DETECTOR)) {
  //  Serial.println("Could not enable shake detector");
  //}
  //if (!bno08x.enableReport(SH2_PERSONAL_ACTIVITY_CLASSIFIER)) {
  //  Serial.println("Could not enable personal activity classifier");
  //}*/
}

void setup_imu(Adafruit_BNO08x* bno08x){
    if (!bno08x->begin_I2C(BNO_ADDRESS)) {
        // if (!bno08x.begin_UART(&Serial1)) {  // Requires a device with > 300 byte
        // UART buffer! if (!bno08x.begin_SPI(BNO08X_CS, BNO08X_INT)) {
        Serial.println("Failed to find BNO08x chip");
        while (1) {
            delay(10);
        }
    }

    Serial.println("BNO08x Found!");
    for (int n = 0; n < bno08x->prodIds.numEntries; n++) {
        Serial.print("Part ");
        Serial.print(bno08x->prodIds.entry[n].swPartNumber);
        Serial.print(": Version :");
        Serial.print(bno08x->prodIds.entry[n].swVersionMajor);
        Serial.print(".");
        Serial.print(bno08x->prodIds.entry[n].swVersionMinor);
        Serial.print(".");
        Serial.print(bno08x->prodIds.entry[n].swVersionPatch);
        Serial.print(" Build ");
        Serial.println(bno08x->prodIds.entry[n].swBuildNumber);
    }

    setReports();

    Serial.println("Reading events");
    delay(100);
}

// This reads a sensor value, if no value was read, returns NULL, if value was read into the 
// provided container, returns the sensor ID of the sensor the value was read from.
sh2_SensorId_t read_imu_no_block(Adafruit_BNO08x* bno08x, ImuData* container){
    if (bno08x->wasReset()) {
        Serial.print("sensor was reset ");
        setReports();
    }

    sh2_SensorValue_t sensorValue;
    if (!bno08x->getSensorEvent(&sensorValue)) {
        return NULL;
    }

    switch (sensorValue.sensorId) {
        case SH2_ACCELEROMETER:
            container->accel.x = sensorValue.un.accelerometer.x;
            container->accel.y = sensorValue.un.accelerometer.y;
            container->accel.z = sensorValue.un.accelerometer.z;
            break;
        case SH2_GYROSCOPE_CALIBRATED:
            container->gyro.x = sensorValue.un.gyroscope.x;
            container->gyro.y = sensorValue.un.gyroscope.y;
            container->gyro.z = sensorValue.un.gyroscope.z;
            break;
        case SH2_LINEAR_ACCELERATION:
            container->linear.x = sensorValue.un.linearAcceleration.x;
            container->linear.y = sensorValue.un.linearAcceleration.y;
            container->linear.z = sensorValue.un.linearAcceleration.z;
            break;
        case SH2_ROTATION_VECTOR:
            container->rotation.real = sensorValue.un.rotationVector.real;
            container->rotation.i = sensorValue.un.rotationVector.i;
            container->rotation.j = sensorValue.un.rotationVector.j;
            container->rotation.k = sensorValue.un.rotationVector.k;
            container->rotation.accuracy = sensorValue.un.rotationVector.accuracy;
            break;
    }
    return sensorValue.sensorId;
}
