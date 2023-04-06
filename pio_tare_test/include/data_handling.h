#ifndef DATAHANDLING_H
#define DATAHANDLING_H

#include <Arduino.h>
#include <WiFiNINA.h>
#include <ArduinoMqttClient.h>
#include <Adafruit_BNO08x.h>
#include <ArduinoJson.h>
#include "secrets.h"
#include "imu.h"
#include "load.h"
#include "status.h"

extern WiFiClient wifiClient;
extern MqttClient mqttClient;

const char broker_ip[]  = "192.168.105.1";
const int  port         = 1883;
const char topic_load[] = "strength";
const char topic_imu[]  = "orientation";
const char topic_status[] = "status";

void setup_data_handler();

void send_data(String data, const char* topic);
void read_data(char* container,
               char* topic);

void format_data_imu(String* container, 
                     unsigned long timestamp,
                     const char* sensor_name,
                     ImuData* imu_data);
void format_data_load(String* container,
                      unsigned long timestamp,
                      const char* sensor_name,
                      LoadValue* load_data);

void format_data_status(String* container,
                      unsigned long timestamp,
                      const char* sensor_name,
                      StatusValue* status_data);

#endif