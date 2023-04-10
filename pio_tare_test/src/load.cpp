// Calibrating the load cell
#include "load.h"

HX711 scale;
float calib_param = 1.0f;

extern bool tare_request;
bool tare_in_process = false;
extern bool tare_next;
extern unsigned int tare_weight;

FlashStorage(calib_storage, float);
FlashStorage(calib_weigth, unsigned int);

void serialFlush(){
  while(Serial.available() > 0) {
    Serial.read();
  }
}

void shift_array(float* array, int len){
  for(int i = len -1; i > 0; i--){
    array[i] = array[i-1];
  }
  array[0] = 0;
}

float avg(float* array, int len){
  float sum = 0;
  for(int i = 0; i < len; i++){
    sum += array[i];
  }
  return sum/len;
}

void tare() {
  tare_in_process = true;
  StatusValue status_val;
  String json_container;

  if (scale.is_ready()) {
    Serial.println("Initialisation... \nremove any weights from the scale.");

    status_val.statuses = (char**)malloc(sizeof(char*)); // allocate 1 status slot
    status_val.statuses[0] = (char*)malloc(strlen(STATUS_TARE_WAIT_EMPTY)); // alocate the string to the slot
    strcpy(status_val.statuses[0], STATUS_TARE_WAIT_EMPTY); // fill the slot

    status_val.nb_statuses = 1;

    format_data_status(&json_container, get_timestamp_now(), SENSOR_NAME, &status_val);
    send_data(json_container, TOPIC_STATUS);
    Serial.println("Waiting on packet 'next'.");

    while(!tare_next){ // wait for next command
      mqttClient.loop();
      delay(100);
    }
    tare_next = false;

    scale.set_scale();
    scale.tare();
    
    Serial.println("Tare done... Put a known weight.");
    
    status_val.statuses[0] = (char*)realloc(status_val.statuses[0], strlen(STATUS_TARE_WAIT_WEIGHT)); // alocate the string to the slot
    strcpy(status_val.statuses[0], STATUS_TARE_WAIT_WEIGHT); // fill the slot

    format_data_status(&json_container, get_timestamp_now(), SENSOR_NAME, &status_val);
    send_data(json_container, TOPIC_STATUS);

    Serial.println("Waiting on packet 'next' with data.");
    while(!tare_next){ // wait for next command
      mqttClient.loop();
      delay(100);
    }
    tare_next = false;

    float reading = scale.get_units(10);
    calib_param = reading/(float)tare_weight;
    scale.set_scale(calib_param);
    calib_storage.write(calib_param);
    calib_weigth.write(tare_weight);

    // unalocate malloced parts
    delete status_val.statuses[0];
    delete status_val.statuses;

    Serial.print("Calibration done with ");
    Serial.print(tare_weight);
    Serial.print("grams, val: ");
    Serial.print(reading, 4);
    Serial.print(", param: ");
    Serial.println(calib_param);
  } else {
    Serial.println("HX711 not found.");
    delay(500);
  }

  tare_in_process = false;
  if(tare_request) tare_request = false;
}

void setup_loadcell() {
  Serial.println("Load cell program starting!");
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  while(!scale.is_ready()){
    Serial.print(".");
    delay(1000);
  }
  float calib;
  calib = calib_storage.read();
  if(calib == 0) calib = 1;
  Serial.print("\nset_scale("); Serial.print(calib); Serial.println(")");
  scale.set_scale(calib);
  //tare();

  Serial.println("time,val,kg,N,temp");
}

bool read_cell_no_block(LoadValue* container){

  if(scale.is_ready()){

    shift_array(container->avg_buf, AVG_BUF_LEN);
    container->avg_buf[0] = scale.get_units(2);

    unsigned int calib_w = calib_weigth.read();
    if(calib_w == 0) calib_w = 1;

    container->raw = avg(container->avg_buf, AVG_BUF_LEN);
    container->kg = (container->raw)/calib_w;
    container->newt = (container->kg)*kg_to_newts;
    Serial.print("read_cell_no_block raw: "); Serial.println(container->avg_buf[0]);
    return true;
  } else {
    return false;
  }
}

//Debug function
void loop_loadcell() {
  while(!scale.is_ready());
  
  LoadValue val_con;
  read_cell_no_block(&val_con);

  Serial.print(",");
  Serial.print(val_con.raw, 4);
  Serial.print(",");
  Serial.print(val_con.kg, 4);
  Serial.print(",");
  Serial.print(val_con.newt, 4);
  Serial.print(",");
  Serial.println("-");
}
