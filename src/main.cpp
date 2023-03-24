#include "Adafruit_CCS811.h"
#include "SPI.h"
#include <PubSubClient.h>
#include "myFunctions.cpp"
#include <WifiManager.h>
#include <Wire.h>

#define MQTT_SERVER "172.16.5.202" // Adresse IP du broker MQTT
#define MQTT_PORT 1883 // Port utilisé par le broker MQTT
#define MQTT_TOPIC "qualite_air" // Topic MQTT sur lequel publier les données
#define MQTT_IDENTIFIANT "pi"
#define MQTT_MOTDEPASSE "Patate123"
const char* mqtt_server = "172.16.5.202";

WiFiManager wm;
Adafruit_CCS811 ccs;
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

const char * SSID = "EcoleDuWeb2.4g";
const char * PASSWORD = "EcoleDuWEB";


void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(SSID);

  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
 
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off".
  // Changes the output state according to the message
  if (String(topic) == "esp32/output") {
    Serial.print("Changing output to ");
    if(messageTemp == "on"){
      Serial.println("on");
    }
    else if(messageTemp == "off"){
      Serial.println("off");
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/output");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);
  if(!ccs.begin()){
    Serial.println("Failed to start sensor!");
    while(1);
  }

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  //Connection au WifiManager
  // String ssIDRandom, PASSRandom;
  // String stringRandom;
  // stringRandom = get_random_string(4).c_str();
  // ssIDRandom = SSID;
  // ssIDRandom = ssIDRandom + stringRandom;
  // stringRandom = get_random_string(4).c_str();
  // PASSRandom = PASSWORD;
  // PASSRandom = PASSRandom + stringRandom;

  // char strToPrint[128];
  // sprintf(strToPrint, "Identification : %s   MotDePasse: %s", ssIDRandom, PASSRandom);
  // Serial.println(strToPrint);

  // if (!wm.autoConnect(ssIDRandom.c_str(), PASSRandom.c_str())) {
  //   Serial.println("Erreur de connexion.");
  // } else {
  //   Serial.println("Connexion Établie.");

  //   // Connection au broker MQTT
  //   mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  //   while (!mqttClient.connected()) {
  //     Serial.println("Tentative de connexion MQTT...");
  //     if (mqttClient.connect("ESP8266Client")) {
  //       Serial.println("Connecté au broker MQTT !");
  //     } else {
  //       Serial.print("Échec de la connexion MQTT avec code d'erreur : ");
  //       Serial.print(mqttClient.state());
  //       Serial.println(" - Nouvelle tentative dans 5 secondes...");
  //       delay(5000);
  //     }
  //   }
  // }
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
   
    // Convert the value to a char array
    char tempString[8] = "Wilfrid";
    client.publish("Qualite", tempString);

    //Serial.println(ccs.geteCO2);
  }

  if(ccs.available()){
    float CO2 = ccs.geteCO2();
    float TVOC = ccs.getTVOC();
    Serial.print("eCO2: ");
    Serial.print(CO2);
    Serial.print(" ppm, TVOC: ");
    Serial.print(TVOC);
    Serial.println(" ppb");

    // Publication des données sur le topic MQTT
  //   char message[64];
  //   sprintf(message, "{\"CO2\": %f, \"TVOC\": %f}", CO2, TVOC);
  //   mqttClient.publish(MQTT_TOPIC, message);
  // }
  // else{
  //   Serial.println("ERROR!");
  }
  delay(1000);
}
// Ce code renvoie sur le terminal le taux de co2 ainsi que le TVOC, et publie les données sur un broker MQTT.
