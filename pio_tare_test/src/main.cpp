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
StatusValue sv; // statuses[0] => general status, statuses[1] => error 1, statuses[2] => error 2 etc...

extern bool tare_request;

void setup() {
    Serial.begin(115200);
    delay(1000); // pause to let serial connect if needed.

    setup_data_handler();
    setup_time();
    setup_loadcell();
    setup_imu(&bno_imu);
    setup_status();
    
    sv.statuses = (char**)malloc(sizeof(char*)); // allocate 1 status slot
    sv.statuses[0] = (char*)malloc(strlen(STATUS_STARTING)); // alocate the string to the slot
    strcpy(sv.statuses[0], STATUS_STARTING); // fill the slot
    sv.nb_statuses = 1;

    delay(STARTUP_DELAY);
}

void loop() {
    unsigned long start_millis = millis();
    loop_count = loop_count + 1;

    if(tare_request){
        ch_status(&sv, 0, STATUS_TARE);
        tare();
    }

    ch_status(&sv, 0, STATUS_ACTIVE);

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

    read_status_no_block(&sv);
    format_data_status(&status_container, get_timestamp_now(), SENSOR_NAME, &sv);
    Serial.println(status_container);
    send_data(status_container, TOPIC_STATUS);
    //}

    unsigned long end_millis = millis();
    unsigned long delta_millis = LOOP_SLEEP - (end_millis - start_millis);
    if(delta_millis > LOOP_SLEEP) delta_millis = 0;
    Serial.print("Sleeping "); Serial.print(delta_millis); Serial.print(" out of "); Serial.print(LOOP_SLEEP); Serial.println(" millis.");
    
    mqttClient.loop();
    LowPower.sleep(delta_millis); // sleep the full delay, minus the time it took to run the loop.
    //delay(delta_millis); // temp to keep the usb connection
}
