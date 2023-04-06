#include <Arduino.h>
#include <time.h>
#include <ArduinoLowPower.h>

#include "npt_time.h"
#include "imu.h"
#include "load.h"
#include "data_handling.h"
#include "battery.h"

unsigned long startMillis;
const char sensor_name[] = "demo_load_cell";
Adafruit_BNO08x bno_imu;
int loop_count = 0;

void setup() {
    setup_data_handler();
    setup_time();
    setup_loadcell();
    setup_imu(&bno_imu);
    setup_battery();
}

String load_container;
String imu_container;
String battery_container;

void loop() {
    loop_count = loop_count + 1;
    LoadValue lv;
    lv.raw = 0.45f;
    lv.kg = 1.5f;
    lv.newt = 2.6f;

    format_data_load(&load_container, get_timestamp_now(), sensor_name, &lv);
    Serial.println(load_container);
    send_data(load_container, topic_load);

    // Send battery info only every 10 seconds (10 loop)
    if(loop_count >= 10) {
        loop_count = 0;
        BatteryValue bv;
        bv.rawADC = 0f;
        bv.voltADC = 0f;
        bv.voltBat = 0f;
        bv.chargeLevel = 0f;

        format_data_battety(&battery_container, get_timestamp_now(), sensor_name, &bv);
        Serial.println(battery_container);
        send_data(load_container, topic_battery);
    }

    delay(1000); // sleep 1 sec*/
}
