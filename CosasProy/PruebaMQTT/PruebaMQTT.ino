#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[50];
int value = 0;

float temperatura = 0;
float humedad = 0;

const int ledPin1 = 2;
const int ledPin2 = 4;
const int ledPin3 = 15;

//PWM
const int ledPin4 = 16;
const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;

//SSID-Password
const char* ssid = "RUBY";
const char* password = "1234456";

//Direccion MQTT Broker IP Address
const char* mqtt_server = "192.168.10.101";

void setup() {
  Serial.begin(115200);
  setup_wifi();

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);

  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(ledPin4, ledChannel);

}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.println("Conectando a:");
  Serial.println("ssid");

  WiFi.begin(ssid, password);

  while (WiFi.status()!= WL_CONNECTED ){
    delay(500);
    Serial.println(".");
  }

  Serial.println(" ");
  Serial.println("WiFi conectado");
  Serial.println("IP Address: ");
  Serial.println(WiFi.localIP());

}

//MQTT -- subscirber: recibe datos
 void callback (char* topic, byte* message, unsigned int length){
  Serial.println("Mensaje recibido en topic: ");
  Serial.print(topic);
  Serial.print(" , Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++){
    Serial.println((char)message[i]);
    messageTemp += (char)message[i];
  
  }
  Serial.println();

  //Output (Topic: esp32/output1)
  if (String(topic) == "esp32/output1")
  {
    Serial.print("Cambio de salida: ");
    if(messageTemp == "off"){
      Serial.println("off");
      digitalWrite(ledPin1, LOW);
    }else if(messageTemp == "on"){
      Serial.println("off");
      digitalWrite(ledPin1, LOW);
    }
  }

  //Output (Topic: esp32/output1)
  if (String(topic) == "esp32/output1")
  {
    Serial.print("Cambio de salida: ");
    if(messageTemp == "off"){
      Serial.println("off");
      digitalWrite(ledPin1, LOW);
    }else if(messageTemp == "on"){
      Serial.println("off");
      digitalWrite(ledPin1, LOW);
    }
  }

  //Output (Topic: esp32/output1)
  if (String(topic) == "esp32/output1")
  {
    Serial.print("Cambio de salida: ");
    if(messageTemp == "off"){
      Serial.println("off");
      digitalWrite(ledPin1, LOW);
    }else if(messageTemp == "on"){
      Serial.println("off");
      digitalWrite(ledPin1, LOW);
    }
  }
}

void reconnect(){
  while(!client.connected()){
    Serial.println("Intentando reconexion MQTT ...");

    if(client.connect("ESP8266Client")){
      Serial.println("Conectado");
      client.subscribe("esp32/output1");
      client.subscribe("esp32/output2");
      client.subscribe("esp32/output3");
      client.subscribe("esp32/output4");
    }else{
      Serial.print("Fallo, rc= ");
      Serial.print(client.state());
      Serial.println("Intentalo de nuevo en 5s");
      delay(5000);
    }
  }
}

void loop() {
  if(!client.connected()){
    reconnect();
  }
  client.loop();

  long now = millis();
  if(now - lastMsg > 100){
    lastMsg = now;

    temperatura = analogRead(36)*(100.0/4095.0);
    char tempString[8];
    dtostrf(temperatura, 1, 2, tempString);
    Serial.println("Temperatura");
    client.publish("esp32/temperature", tempString);
    
    humedad = 289;
    char humString[8];
    dtostrf(humedad, 1, 2, humString);
    Serial.println("Humedad");
    client.publish("esp32/humidity", humString);
  }

}
