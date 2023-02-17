#include <Wire.h>
#include "Adafruit_CCS811.h"
#include "SPI.h"

Adafruit_CCS811 ccs;

void setup() {
  Serial.begin(9600);
  if(!ccs.begin()){
    Serial.println("Failed to start sensor!");
    while(1);
  }
}

void loop() {
  if(ccs.available()){
    float CO2 = ccs.geteCO2();
    float TVOC = ccs.getTVOC();
    Serial.print("eCO2: ");
    Serial.print(CO2);
    Serial.print(" ppm, TVOC: ");
    Serial.print(TVOC);
    Serial.println(" ppb");
  }
  else{
    Serial.println("ERROR!");
  }
  delay(1000);
}
// Ce code renvoie sur le terminal le taux de co2 ainsi qu le TVOC