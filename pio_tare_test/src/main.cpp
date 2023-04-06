#include <Arduino.h>
#include <time.h>
#include <ArduinoLowPower.h>

#include "npt_time.h"
#include "imu.h"
#include "load.h"
#include "data_handling.h"
#include "generic_const.h"

Adafruit_BNO08x bno_imu;

String load_container;
String imu_container;
LoadValue lval;
ImuData idata;

void setup() {
    Serial.begin(115200);
    while (!Serial)
        delay(10); // will pause Zero, Leonardo, etc until serial console opens

    setup_data_handler();
    setup_time();
    setup_loadcell();
    setup_imu(&bno_imu);
    delay(STARTUP_DELAY);
}

void loop() {
    unsigned long start_millis = millis();

    /*read_cell_no_block(&lval);
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

    unsigned long end_millis = millis();
    unsigned long delta_millis = LOOP_SLEEP - (end_millis - start_millis);
    //LowPower.idle(delta_millis); // sleep the full delay, minus the time it took to run the loop.
    delay(delta_millis); // temp because LowPower doesn't work.
}
