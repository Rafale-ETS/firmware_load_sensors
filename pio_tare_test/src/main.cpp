#include <Arduino.h>
#include <time.h>
#include <ArduinoLowPower.h>

#include "npt_time.h"
#include "imu.h"
#include "load.h"
#include "data_handling.h"
#include "status.h"
#include "generic_const.h"

Adafruit_BNO08x bno_imu;
int loop_count = 0;

String load_container;
String imu_container;
String status_container;
LoadValue lval;
ImuData idata;
StatusValue sv;

extern bool tare_request;

void setup() {
    Serial.begin(115200);
    delay(1000); // pause to let serial connect if needed.

    setup_data_handler();
    setup_time();
    setup_loadcell();
    setup_imu(&bno_imu);
    setup_status();
    
    delay(STARTUP_DELAY);
}

void loop() {
    unsigned long start_millis = millis();
    loop_count = loop_count + 1;

    if(tare_request)
        tare();

    read_cell_no_block(&lval);
    Serial.print("Load-Cell: "); Serial.println(lval.raw);
    format_data_load(&load_container, get_timestamp_now(), SENSOR_NAME, &lval);
    Serial.println(load_container);
    send_data(load_container, TOPIC_LOAD);

    read_imu_no_block(&bno_imu, &idata);
    format_data_imu(&imu_container, get_timestamp_now(), SENSOR_NAME, &idata);
    Serial.println(imu_container);
    send_data(imu_container, TOPIC_IMU);

    // Send status info only every 10 seconds (10 loop)
    //if(loop_count >= 10) {
    loop_count = 0;
    //add_status(&sv, STATUS_ACTIVE);

    read_status_no_block(&sv);
    format_data_status(&status_container, get_timestamp_now(), SENSOR_NAME, &sv);
    Serial.println(status_container);
    send_data(status_container, TOPIC_STATUS);
    //}

    unsigned long end_millis = millis();
    unsigned long delta_millis = LOOP_SLEEP - (end_millis - start_millis);
    if(delta_millis > LOOP_SLEEP) delta_millis = 0;
    //LowPower.idle(delta_millis); // sleep the full delay, minus the time it took to run the loop.
    Serial.print("Sleeping "); Serial.print(delta_millis); Serial.print(" out of "); Serial.print(LOOP_SLEEP); Serial.println(" millis.");
    
    mqttClient.loop();
    delay(delta_millis); // temp because LowPower doesn't work.
}
