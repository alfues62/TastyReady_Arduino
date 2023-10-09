#include <M5Stack.h>

#include <ArduinoMqttClient.h>
#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif

//#include "arduino_secrets.h"
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = "POCO F3";    // your network SSID (name)
char pass[] = "carlitos";    // your network password (use for WPA, or use as key for WEP)

// To connect with SSL/TLS:
// 1) Change WiFiClient to WiFiSSLClient.
// 2) Change port value from 1883 to 8883.
// 3) Change broker value to a server with a known SSL/TLS root certificate 
//    flashed in the WiFi module.

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "test.mosquitto.org";
int        port     = 1883;
const char topic[]  = "bolando/practica/timbre";

const long interval = 1000;
unsigned long previousMillis = 0;
int count = 0;

void setup() {
 //Initialize serial and wait for port to open:
  M5.begin();
  Serial.begin(115200);
  M5.Lcd.setRotation(3);
   
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // attempt to connect to WiFi network:
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }

  Serial.println("You're connected to the network");
  Serial.println();

// You can provide a unique client ID, if not set the library uses Arduino-millis()
  // Each client must have a unique client ID
  // mqttClient.setId("clientId");

  // You can provide a username and password for authentication
  // mqttClient.setUsernamePassword("username", "password");

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();

  mqttClient.onMessage(onMqttMessage); // set the message receive callback
 Serial.print("Subscribing to topic: ");
 Serial.println("bolando/practica/abrir");
 Serial.println();
 mqttClient.subscribe("bolando/practica/abrir");
}

void onMqttMessage(int messageSize) {
 if (mqttClient.available()) {
 Serial.print("abrir puerta");
 }
 Serial.println();
 Serial.println();
 }

void loop() {
 //call poll() regularly to allow the library to receive MQTT messages and
 //send MQTT keep alives which avoids being disconnected by the broker
 mqttClient.poll();
 //avoid having delays in loop, we'll use the strategy from BlinkWithoutDelay
 //see: File -> Examples -> 02.Digital -> BlinkWithoutDelay for more info
 unsigned long currentMillis = millis();
 if (currentMillis - previousMillis >= interval) {
   M5.BtnA.read();
   if(M5.BtnA.wasPressed()){
    Serial.println("Boton A pulsado");
    previousMillis = currentMillis;
    mqttClient.beginMessage(topic);
    mqttClient.print(count);
    mqttClient.endMessage();
    Serial.println();
    count++;
  }  
 }
