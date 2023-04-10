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
LoadValue lv;

void setup() {
    Serial.begin(115200);
    while (!Serial)
        delay(10); // will pause Zero, Leonardo, etc until serial console opens

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

    /*
    read_cell_no_block(&lval);
    Serial.print("Load-Cell: "); Serial.println(lval.raw);
    format_data_load(&load_container, get_timestamp_now(), SENSOR_NAME, &lval);
    Serial.println(load_container);
    send_data(load_container, TOPIC_LOAD);

    read_imu_no_block(&bno_imu, &idata);
    format_data_imu(&imu_container, get_timestamp_now(), SENSOR_NAME, &idata);
    Serial.println(imu_container);
    send_data(imu_container, TOPIC_IMU);
    */
    
    if(is_tare_requested()){
        tare();
    }

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

    unsigned long end_millis = millis();
    unsigned long delta_millis = LOOP_SLEEP - (end_millis - start_millis);
    //LowPower.idle(delta_millis); // sleep the full delay, minus the time it took to run the loop.
    
    mqttClient.loop();
    delay(delta_millis); // temp because LowPower doesn't work.
}
