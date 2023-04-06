#include <Arduino.h>
#include <time.h>
#include <ArduinoLowPower.h>

#include "npt_time.h"
#include "imu.h"
#include "load.h"
#include "data_handling.h"
#include "status.h"

unsigned long startMillis;
const char sensor_name[] = "demo_load_cell";
Adafruit_BNO08x bno_imu;
int loop_count = 0;

void setup() {
    setup_data_handler();
    setup_time();
    setup_loadcell();
    setup_imu(&bno_imu);
    setup_status();
}

String load_container;
String imu_container;
String status_container;

void loop() {
    loop_count = loop_count + 1;
    LoadValue lv;
    lv.raw = 0.45f;
    lv.kg = 1.5f;
    lv.newt = 2.6f;

    format_data_load(&load_container, get_timestamp_now(), sensor_name, &lv);
    Serial.println(load_container);
    send_data(load_container, topic_load);

    // Send status info only every 10 seconds (10 loop)
    if(loop_count >= 10) {
        loop_count = 0;
        StatusValue sv;
        // TODO : Add status
        sv.batterieVoltage = 0f;
        sv.chargeLevel = 0f;

        format_data_status(&status_container, get_timestamp_now(), sensor_name, &sv);
        Serial.println(status_container);
        send_data(status_container, topic_status);
    }

    delay(1000); // sleep 1 sec*/
}
