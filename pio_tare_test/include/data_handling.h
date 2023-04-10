#ifndef DATAHANDLING_H
#define DATAHANDLING_H

#include <Arduino.h>
#include <WiFiNINA.h>
//#include <ArduinoMqttClient.h>
#include <PubSubClient.h>
#include <Adafruit_BNO08x.h>
#include <ArduinoJson.h>
#include "load.h"
#include "secrets.h"
#include "imu.h"
#include "status.h"
#include "generic_const.h"

extern WiFiClient wifiClient;
extern PubSubClient mqttClient;

const char BROKER_IP[]  = "192.168.105.1";
const int  BROKER_PORT  = 1883;
const char TOPIC_LOAD[] = "strength";
const char TOPIC_IMU[]  = "orientation";
const char TOPIC_CONTROL[] = "control";
const char TOPIC_STATUS[] = "status";

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
                      
//Load cell
void onMqttMessage(char* topic, uint8_t* payload, unsigned int length);

void validate_WIFI();

#endif