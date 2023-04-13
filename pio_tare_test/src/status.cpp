/*
Read battery voltage on MKR WIFI 1010 !
This code is an adaptation of the code on the site https://docs.arduino.cc/tutorials/mkr-wifi-1010/mkr-battery-app-note
*/
#include "status.h"

float rawADC; // unprocessed ADC value
float voltADC; // ADC converted into voltage
float voltBat; // calculated voltage on battery
float chargeLevel; // charge level of the battery in %
int max_Source_voltage; // upper source voltage for the battery

void setup_status() {
Serial.begin(9600);               // start Serial port with a baudrate of 9600bps

analogReference(AR_DEFAULT);      // the upper value of ADC is set to 3.3V
analogReadResolution(12);         // this will give us 4096 (2^12) levels on the ADC

// configure BQ24195 PMIC
PMIC.begin();                                      // start the PMIC I2C connection
PMIC.enableBoostMode();                            // boost battery output to 5V

PMIC.setMinimumSystemVoltage(BATTERY_EMPTY_VOLTAGE); // set the minimum battery output to 3.5V
PMIC.setChargeVoltage(BATTERY_FULL_VOLTAGE);         // set battery voltage at full charge

PMIC.setChargeCurrent(BATTERY_CAPACITY/2);          // set battery current to C/2 in amps

PMIC.enableCharge();                               // enable charging of battery

// The formula for calculating the output of a voltage divider is
// Vout = (Vsource x R2)/(R1 + R2)
// If we consider that 3.3V is the maximum that can be applied to Vout then the maximum source voltage is calculated as
max_Source_voltage = (3.3 * (R1 + R2))/R2;
}

void read_status_no_block(StatusValue* container) {
    rawADC = analogRead(ADC_BATTERY); // the value obtained directly at the PB09 input pin
    voltADC = rawADC*(3.3/4095.0); // convert ADC value to the voltage read at the pin
    voltBat = voltADC*(max_Source_voltage/3.3); // we cannot use map since it requires int inputs/outputs
    chargeLevel = (voltBat - BATTERY_EMPTY_VOLTAGE)*(100.0)/(BATTERY_FULL_VOLTAGE - BATTERY_EMPTY_VOLTAGE); 
    container->batteryVoltage = voltBat;
    container->chargeLevel = chargeLevel;
}

// Debug function
void loop_status() {

    StatusValue val_con;
    read_status_no_block(&val_con);

    Serial.print("The ADC on PB09 reads a value of ");
    Serial.print(rawADC);
    Serial.print(" which is equivialent to ");
    Serial.print(voltADC);
    Serial.print("V. This means the battery voltage is ");
    Serial.print(voltBat);
    Serial.print("V. Which is equivalent to a charge level of ");
    Serial.print(chargeLevel);
    Serial.println("%.");
    delay(2000);
}

void add_status(StatusValue* container, const char* status){
    // create a temp array
    char** temp_statuses = (char**)realloc(container->statuses, (container->nb_statuses + 1) * sizeof(char*));
    
    if(temp_statuses == NULL){
        // realloc failed
        Serial.print("Couldn't allocate more memory to status array. Nb Statuses:");
        Serial.println(container->nb_statuses);
    } else {
        // realloc success, add the new status
        temp_statuses[container->nb_statuses] = (char*)malloc(strlen(status)); 
        strcpy(temp_statuses[container->nb_statuses], status);
        container->nb_statuses++;
       
        // assigne temp array to real one.
        container->statuses = temp_statuses;
    }
}

void rm_status(StatusValue* container, const char* status){
    for(int i = 0; i < container->nb_statuses; i++){
        if(strcmp(status, container->statuses[i]) == 0){
            // we have a match to remove
            delete container->statuses[i];
            container->nb_statuses--;

            if(i < container->nb_statuses){
                // we have removed a status that is not the last one in the list, we gotta move everyone.
                for(i++;i < container->nb_statuses; i++){
                    container->statuses[i] = container->statuses[i + 1];
                }
            }

            char** new_statuses = (char**)realloc(container->statuses, (container->nb_statuses * sizeof(char*)));
            // assigne temp array to real one.
            container->statuses = new_statuses;
        }
    }
}

void ch_status(StatusValue* container, int id, const char* new_status){
    char* tmp_status = (char*)realloc(container->statuses[id], strlen(new_status)); // alocate the string to the slot
    if(tmp_status == NULL){
        //failed to realloc
        Serial.print("Failled to realloc status string to change it. Original status: ");
        Serial.print(container->statuses[0]);
    } else {
        // realloc success
        strcpy(tmp_status, new_status); // fill the slot
        container->statuses[id] = tmp_status;
    }
}