/*
  Read battery voltage on MKR WIFI 1010 !
  This code is an adaptation of the code on the site https://docs.arduino.cc/tutorials/mkr-wifi-1010/mkr-battery-app-note
*/
// Include the library for the BQ24195 IC
#include <BQ24195.h>
#include <Arduino.h>
#include "status.h"

float rawADC; // unprocessed ADC value
float voltADC; // ADC converted into voltage
float voltBat; // calculated voltage on battery
float chargeLevel; // charge level of the battery in %
int max_Source_voltage; // upper source voltage for the battery

unsigned long start_time = 0;

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

void read_status_no_block(BatteryValue* container) {
    rawADC = analogRead(ADC_BATTERY); // the value obtained directly at the PB09 input pin
    voltADC = rawADC*(3.3/4095.0); // convert ADC value to the voltage read at the pin
    voltBat = voltADC*(max_Source_voltage/3.3); // we cannot use map since it requires int inputs/outputs
    chargeLevel = (voltBat - BATTERY_EMPTY_VOLTAGE)*(100.0)/(BATTERY_FULL_VOLTAGE - BATTERY_EMPTY_VOLTAGE); 
    container->batteryVoltage = voltBat;
    container->chargeLevel = chargeLevel;
}

// Debug function
void loop_status() {
    unsigned long time = millis() - start_time;

    StatusValue val_con;
    read_status_no_block(&val_con);

    Serial.print(time);
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
