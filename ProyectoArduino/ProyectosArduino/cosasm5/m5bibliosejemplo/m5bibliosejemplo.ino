#include "DHT.h"
#include "PubSubClient.h" // Connect and publish to the MQTT broker
//#include <ArduinoMqttClient.h>

#include "WiFi.h" // Enables the ESP32 to connect to the local network (via WiFi)
#define DHTPIN 26  // Pin connected to the DHT sensor

#define DHTTYPE DHT11  // DHT11 or DHT22
DHT dht(DHTPIN, DHTTYPE);

// WiFi
const char* ssid = "POCO F3";                 // Your personal network SSID
const char* wifi_password = "carlitos"; // Your personal network password

// MQTT
const char* mqtt_server = "192.168.195.139";  // IP of the MQTT broker
const char* topic = "bolando/Cullera/Iluminacion";
const char* humidity_topic = "bolando/pista1/humedad";
const char* temperature_topic = "bolando/pista1/temperatura";
const char* mqtt_username = "carlos"; // MQTT username
const char* mqtt_password = "carlos"; // MQTT password
const char* clientID = "BOLANDO"; // MQTT client ID

// Initialise the WiFi and MQTT Client objects
WiFiClient wifiClient;
// 1883 is the listener port for the Broker
PubSubClient client(mqtt_server, 1883, wifiClient); 

// variable for storing the potentiometer value
const int potPin = 25;
int potValue = 0;
int luminosidad = 0;
int IlumMin = 4095;
int IlumMax = 0;

int antes = 0;

void connect_MQTT(){
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Connect to the WiFi
  WiFi.begin(ssid, wifi_password);

  // Wait until the connection has been confirmed before continuing
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Debugging - Output the IP Address of the ESP8266
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Connect to MQTT Broker
  // client.connect returns a boolean value to let us know if the connection was successful.
  // If the connection is failing, make sure you are using the correct MQTT Username and Password (Setup Earlier in the Instructable)
  if (client.connect(clientID, mqtt_username, mqtt_password)) {
    Serial.println("Connected to MQTT Broker!");
    Serial.println("Connected now subscribing");
    boolean r= client.subscribe("bolando/Cullera/Iluminacion");
  }
  else {
    Serial.println("Connection to MQTT Broker failed...");
  }
}

// Custom function to connet to the MQTT broker via WiFi
void setup() {
  Serial.begin(115200);
  dht.begin();
   pinMode(3, OUTPUT);

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
  connect_MQTT();
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageLuz;
  
  for (int i = 0; i < length; i++) {
    //Serial.print((char)message[i]);
    messageLuz += (char)message[i];
  }

  Serial.print(messageLuz);
  if (messageLuz == "false"){
    digitalWrite(3, HIGH);
    }
  else if (messageLuz == "true") {
    digitalWrite(3, LOW);
    }
}

void loop() {
  client.loop();
  //Serial.setTimeout(2000);

  potValue = analogRead(potPin);
  luminosidad = 100 * IlumMin / (IlumMin - IlumMax) - potValue * 100 / (IlumMin - IlumMax);
  
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.println(" %");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" *C");

  // MQTT can only transmit strings
  String hs="Hum: "+String((float)h)+" % ";
  String ts="Temp: "+String((float)t)+" C ";

  // PUBLISH to the MQTT Broker (topic = Temperature, defined at the beginning)
  if (client.publish(temperature_topic, String(t).c_str())) {
    Serial.println("Temperature sent!");
  }
  // Again, client.publish will return a boolean value depending on whether it succeded or not.
  // If the message failed to send, we will try again, as the connection may have broken.
  else {
    Serial.println("Temperature failed to send. Reconnecting to MQTT Broker and trying again");
    client.connect(clientID, mqtt_username, mqtt_password);
    delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
    client.publish(temperature_topic, String(t).c_str());
  }

  // PUBLISH to the MQTT Broker (topic = Humidity, defined at the beginning)
  if (client.publish(humidity_topic, String(h).c_str())) {
    Serial.println("Humidity sent!");
  }
  // Again, client.publish will return a boolean value depending on whether it succeded or not.
  // If the message failed to send, we will try again, as the connection may have broken.
  else {
    Serial.println("Humidity failed to send. Reconnecting to MQTT Broker and trying again");
    client.connect(clientID, mqtt_username, mqtt_password);
    delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
    client.publish(humidity_topic, String(h).c_str());
  }
    // disconnect from the MQTT broker
  delay(5000);       // print new values every 1 Minute

}
