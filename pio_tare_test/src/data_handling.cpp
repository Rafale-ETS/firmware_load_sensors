#include "data_handling.h"

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

bool tare_request = false;
bool tare_next = false;
unsigned int tare_weight = 0;
extern bool tare_in_process;

void connect_WiFi(){
    if(WiFi.status() != WL_CONNECTED){
        Serial.print("Attempting to connect to WPA SSID: ");
        Serial.println(HUB_SSID);
        while(WiFi.begin(HUB_SSID, HUB_PASS) != WL_CONNECTED) {
            // failed, retry.
            Serial.print(".");
            delay(5000);
        }
        WiFi.lowPowerMode();
        Serial.println("\nConnection success!\n");
    } else {
        Serial.print("Already connected to WiFi: ");
    }
}

void connect_broker(){
    Serial.print("Attempting to contact MQTT brocker at: ");
    Serial.println(BROKER_IP);
    mqttClient.setServer(BROKER_IP, BROKER_PORT);
    if(!mqttClient.connect(SENSOR_NAME)){
        // failed, retry
        Serial.print("q");
        delay(1000);
    }
    Serial.println("Broker found!\n");
    
    // set the message receive callback
    mqttClient.setBufferSize(512);
    mqttClient.setCallback(onMqttMessage);
    mqttClient.subscribe(TOPIC_CONTROL);
}

void setup_data_handler(){
    connect_WiFi();
    connect_broker();
}

void send_data(String data, const char* topic){
    char payload[data.length()+1];
    data.toCharArray(payload, data.length()+1);

    mqttClient.publish(topic, payload);
}

void validate_WIFI(){
    if(WiFi.status() != WL_CONNECTED){
        setup_data_handler();
    } else {
        Serial.print("Still connected to WiFi.");
    }
}

void format_data_imu(String* container, 
                     unsigned long timestamp,
                     const char* sensor_name,
                     ImuData* imu_data)
{
    StaticJsonDocument<384> doc;

    doc["time"] = timestamp;
    doc["type"] = "imu";
    doc["name"] = String(sensor_name);

    JsonObject data = doc.createNestedObject("data");

    JsonObject data_accel = data.createNestedObject("accel");
    data_accel["x"] = imu_data->accel.x;
    data_accel["y"] = imu_data->accel.y;
    data_accel["z"] = imu_data->accel.z;

    JsonObject data_gyro = data.createNestedObject("gyro");
    data_gyro["x"] = imu_data->gyro.x;
    data_gyro["y"] = imu_data->gyro.y;
    data_gyro["z"] = imu_data->gyro.z;

    JsonObject data_linear = data.createNestedObject("linear");
    data_linear["x"] = imu_data->linear.x;
    data_linear["y"] = imu_data->linear.y;
    data_linear["z"] = imu_data->linear.z;

    JsonObject data_rot = data.createNestedObject("rot");
    data_rot["i"] = imu_data->rotation.i;
    data_rot["j"] = imu_data->rotation.j;
    data_rot["k"] = imu_data->rotation.k;
    data_rot["real"] = imu_data->rotation.real;
    data_rot["accuracy"] = imu_data->rotation.accuracy;

    String json;
    serializeJson(doc, json);
    *container = json;
}

void format_data_load(String* container,
                      unsigned long timestamp,
                      const char* sensor_name,
                      LoadValue* load_data)
{
    StaticJsonDocument<192> doc;

    doc["time"] = timestamp;
    doc["type"] = "load_cell";
    doc["name"] = String(sensor_name);

    JsonObject data = doc.createNestedObject("data");
    data["load_raw"] = load_data->raw;
    data["load_kg"] = load_data->kg;
    data["load_N"] = load_data->newt;

    String json;
    serializeJson(doc, json);
    *container = json;
}

void format_data_status(String* container,
                      unsigned long timestamp,
                      const char* sensor_name,
                      StatusValue* status_data)
{
    StaticJsonDocument<192> doc;

    doc["time"] = timestamp;
    doc["type"] = "status";
    doc["name"] = String(sensor_name);

    JsonObject data = doc.createNestedObject("data");
    JsonArray status = data.createNestedArray("status");
    for(unsigned int i = 0; i < status_data->nb_statuses; i++){
        status.add(String(status_data->statuses[i]));
    }
    data["nb_statuses"] = status_data->nb_statuses;
    data["battery_voltage"] = status_data->batteryVoltage;
    data["charge_level"] = status_data->chargeLevel;

    String json;
    serializeJson(doc, json);
    *container = json;
}

void onMqttMessage(char* topic, uint8_t* payload, unsigned int length){
    // we received a message, print out the topic and contents
    Serial.print("Received a message with topic '");
    Serial.print(topic);
    Serial.print("', length ");
    Serial.print(length);
    Serial.println(" bytes:");

    StaticJsonDocument<256> doc;
    String json_data = String((char*) payload);
    Serial.println(json_data);

    DeserializationError error = deserializeJson(doc, json_data);
    
    if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return;
    }

    //unsigned long timestamp = doc["time"]; // 1351824120
    //const char* pkt_type = doc["type"]; // "control"
    const char* dest_name = doc["name"]; // "default"

    if(strcmp(dest_name, SENSOR_NAME) == 0){
        JsonArray commands = doc["commands"];

        int nb_commands = commands.size();
        for(int i = 0; i < nb_commands; i++){
            const char* command = commands[i];

            if(strcmp(command, "tare") == 0) {
                tare_request = true;
                Serial.println("Tare requested!");
            }else if(strcmp(command, "idle") == 0){
                // put device in idle
                Serial.println("Idle requested!");
            }else if(strcmp(command, "start") == 0){
                // put device in start
                Serial.println("Start requested!");
            }else if(strcmp(command, "cancel") == 0){
                // cancel current command / return to regular mode
                Serial.println("Cancel requested!");
            }else if(strcmp(command, "next") == 0){
                // go to next step (tare process)
                if(tare_in_process){
                    tare_next = true;
                    tare_weight = doc["data"];
                }
            }
        }
    }
}

