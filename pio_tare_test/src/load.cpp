// Calibrating the load cell
#include "load.h"

HX711 scale;
int calib_weight = 30000; //g
float calib_param = 1.0f;

unsigned long start_time = 0;

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
  bool tar_done = false;
  while(!tar_done){
    if (scale.is_ready()) {
      Serial.println("Initialisation... \nremove any weights from the scale.\nPress any key to go on...");
      while(Serial.available() == 0);
      serialFlush();
      scale.set_scale();
      scale.tare();
      
      Serial.println("Tare done... Put a known weight. \nEnter the weight value in grams.");
      while(Serial.available() == 0);
      calib_weight = Serial.parseInt();
      serialFlush();
      float reading = scale.get_units(10);
      calib_param = reading/(float)calib_weight;
      scale.set_scale(calib_param);
      calib_storage.write(calib_param);

      Serial.print("Calibration done with ");
      Serial.print(calib_weight);
      Serial.print("grams, val: ");
      Serial.print(reading, 4);
      Serial.print(", param: ");
      Serial.println(calib_param);

      tar_done = true;
    }
    else {
      Serial.println("HX711 not found.");
      delay(500);
    }
  }
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
    container->avg_buf[0] = scale.get_units(5);

    container->raw = avg(container->avg_buf, AVG_BUF_LEN);
    container->kg = (container->raw)/calib_weight;
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

  unsigned long time = millis() - start_time;
  
  LoadValue val_con;
  read_cell_no_block(&val_con);

  Serial.print(time);
  Serial.print(",");
  Serial.print(val_con.raw, 4);
  Serial.print(",");
  Serial.print(val_con.kg, 4);
  Serial.print(",");
  Serial.print(val_con.newt, 4);
  Serial.print(",");
  Serial.println("-");
}
