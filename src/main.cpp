#include "Adafruit_CCS811.h"
#include "SPI.h"
#include <PubSubClient.h>
#include <WifiManager.h>
#include <Wire.h>

#define MQTT_SERVER "172.16.5.202" // Adresse IP du broker MQTT
#define MQTT_PORT 1883 // Port utilisé par le broker MQTT
#define MQTT_TOPIC "qualite_air" // Topic MQTT sur lequel publier les données
#define MQTT_IDENTIFIANT "pi"
#define MQTT_MOTDEPASSE "Patate123"

WiFiManager wm;
Adafruit_CCS811 ccs;
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void setup() {
  Serial.begin(9600);
  if(!ccs.begin()){
    Serial.println("Failed to start sensor!");
    while(1);
  }

  //Connection au WifiManager
  String ssIDRandom, PASSRandom;
  String stringRandom;
  stringRandom = get_random_string(4).c_str();
  ssIDRandom = SSID;
  ssIDRandom = ssIDRandom + stringRandom;
  stringRandom = get_random_string(4).c_str();
  PASSRandom = PASSWORD;
  PASSRandom = PASSRandom + stringRandom;

  char strToPrint[128];
  sprintf(strToPrint, "Identification : %s   MotDePasse: %s", ssIDRandom, PASSRandom);
  Serial.println(strToPrint);

  if (!wm.autoConnect(ssIDRandom.c_str(), PASSRandom.c_str())) {
    Serial.println("Erreur de connexion.");
  } else {
    Serial.println("Connexion Établie.");
    // Connection au broker MQTT
    mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
    while (!mqttClient.connected()) {
      Serial.println("Tentative de connexion MQTT...");
      if (mqttClient.connect("ESP8266Client")) {
        Serial.println("Connecté au broker MQTT !");
      } else {
        Serial.print("Échec de la connexion MQTT avec code d'erreur : ");
        Serial.print(mqttClient.state());
        Serial.println(" - Nouvelle tentative dans 5 secondes...");
        delay(5000);
      }
    }
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

    // Publication des données sur le topic MQTT
    char message[64];
    sprintf(message, "{\"CO2\": %f, \"TVOC\": %f}", CO2, TVOC);
    mqttClient.publish(MQTT_TOPIC, message);
  }
  else{
    Serial.println("ERROR!");
  }
  delay(1000);
}
// Ce code renvoie sur le terminal le taux de co2 ainsi que le TVOC, et publie les données sur un broker MQTT.
