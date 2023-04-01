#include <Arduino.h>
#include <time.h>
#include <ArduinoLowPower.h>

#include "npt_time.h"
#include "imu.h"
#include "load.h"
#include "data_handling.h"

unsigned long startMillis;
const char sensor_name[] = "demo_load_cell";
Adafruit_BNO08x bno_imu;

void setup() {
    setup_data_handler();
    setup_time();
    setup_loadcell();
    setup_imu(&bno_imu);
}

String load_container;
String imu_container;

void loop() {
    LoadValue lv;
    lv.raw = 0.45f;
    lv.kg = 1.5f;
    lv.newt = 2.6f;

    format_data_load(&load_container, get_timestamp_now(), sensor_name, &lv);
    Serial.println(load_container);
    send_data(load_container, topic_load);

    delay(1000); // sleep 1 sec*/
}
