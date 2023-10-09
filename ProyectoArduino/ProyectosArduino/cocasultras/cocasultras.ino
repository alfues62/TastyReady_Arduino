#include "WiFi.h"
#include "SD.h"
#include "ArduinoMqttClient"

#include "M5Stack.h"

char ssid[] = "MiFibra-5182";    // your network SSID (name)
char pass[] = "mEF5WTPx";    // your network password (use for WPA, or use

const char broker[] = "test.mosquitto.org";
int        port     = 1883;
const char topic[]  = "examenFuset/sensorUltras";

const long interval = 1000;
unsigned long previousMillis = 0;
int count = 0;

#define Trigger 33   //Pin digital 2 para el Trigger del sensor
#define Echo 32   //Pin digital 3 para el Echo del sensor

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

void setup() {
  M5.begin();
  Serial.begin(115200);
  M5.Lcd.setRotation(3);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  pinMode(Trigger, OUTPUT); //pin como salida
  pinMode(Echo, INPUT);  //pin como entrada
  digitalWrite(Trigger, LOW);//Inicializamos el pin con 0

  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }

  Serial.println("You're connected to the network");
  Serial.println();

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
  Serial.println("examenFuset/sensorUltras");
  Serial.println();
  mqttClient.subscribe("examenFuset/sensorUltras");
  
}

void onMqttMessage(int messageSize) {
 if (mqttClient.available()) {
 Serial.print("abrir puerta");
 }
 Serial.println();
 Serial.println();
 }

void loop()
{
   mqttClient.poll();
  
  long t; //timepo que demora en llegar el eco
  long d; //distancia en centimetros
  unsigned long currentMillis = millis();


  digitalWrite(Trigger, HIGH);
  delayMicroseconds(10);          //Enviamos un pulso de 10us
  digitalWrite(Trigger, LOW);
  
  t = pulseIn(Echo, HIGH); //obtenemos el ancho del pulso
  d = t/59;             //escalamos el tiempo a una distancia en cm
  
  Serial.print("Distancia: ");
  Serial.print(d);      //Enviamos serialmente el valor de la distancia
  Serial.print("cm");
  Serial.println();

  previousMillis = currentMillis;

  mqttClient.beginMessage(topic);
  mqttClient.print(d);
  mqttClient.endMessage();
  Serial.println();
  
  delay(5000);          //Hacemos una pausa de 100ms

  
}
