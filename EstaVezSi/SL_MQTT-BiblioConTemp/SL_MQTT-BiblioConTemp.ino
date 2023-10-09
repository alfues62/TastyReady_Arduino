       //******************************************************************************************
//
//  Codigo MQTT - Biblioteca - ESP32_1.
//
//  Leds Verdes y Medidsas de Temperatura.
//
//******************************************************************************************
#include "PubSubClient.h"
#include "WiFi.h"
#include "DHT.h"         

#define DHTTYPE DHT11

// WiFi -- Declaración
const char* ssid = "alfues62";          // SSID
const char* wifi_password = "alfues62"; // password

// MQTT -- Servidor.
const char* mqtt_server = "192.168.66.86";  // IP del MQTT broker

//MQTT -- Topicos.
const char* lum1_topic = "smarlib/silla1/led";
const char* lum2_topic = "smarlib/silla2/led";
const char* lum3_topic = "smarlib/silla3/led";
const char* lum4_topic = "smarlib/silla4/led";

const char* temperature_topic = "smarlib/biblioteca/temperatura";

//MQTT -- Info del Usuario.
const char* mqtt_username = "smartlibrary"; // MQTT username.
const char* mqtt_password = "smartlibrary"; // MQTT password.

const char* clientID = "SMARTLIBRARY";      // MQTT client ID.

//Declaración de Pines de LEDs
#define ledPinV1 25
#define ledPinV2 26
#define ledPinV3 33
#define ledPinV4 32

//Declaración Pines dht
#define DHTPIN 18

//Declaración de variables.
float temperatura = 0;

DHT dht(DHTPIN, DHTTYPE);

WiFiClient wifiClient;                              // Inicializa el WiFi y MQTT Client.
PubSubClient client(mqtt_server, 1883, wifiClient); // 1883 como listener.

/*-------------------------------------------------------------------------

  connect_MQTT()

  Realiza la conexión con el WiFi especificado y con el Broker MQTT
  de la Raspberry Pi. 

-------------------------------------------------------------------------*/
void connect_MQTT(){
  Serial.print("Connecting to ");
  Serial.println(ssid);

  dht.begin(); // Activa el sensor DHT

  // Connect to the WiFi
  WiFi.begin(ssid, wifi_password);

  // Espera hasta que la coneción haya sido realizada
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Debug - Output IP Address ESP32
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (client.connect(clientID, mqtt_username, mqtt_password)) {
    Serial.println("Connected to MQTT Broker!");
    Serial.println("Connected now subscribing");
    client.subscribe("smarlib/silla1/led");
    client.subscribe("smarlib/silla2/led");
    client.subscribe("smarlib/silla3/led");
    client.subscribe("smarlib/silla4/led");
  }
  else {
    Serial.println("Connection to MQTT Broker failed...");
  }
}

/*-------------------------------------------------------------------------

  setup()

  Realiza la configuración inicial y llama a connect_MQTT()

-------------------------------------------------------------------------*/
void setup() {
  Serial.begin(115200);
    
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  //PinMode -- Declaraciones pins LEDs.
  pinMode(ledPinV1, OUTPUT);
  pinMode(ledPinV2, OUTPUT);
  pinMode(ledPinV3, OUTPUT);
  pinMode(ledPinV4, OUTPUT);
    
  //Inicializa todos los LEDs verdes para que esten abiertos.
  digitalWrite(ledPinV1, HIGH);
  digitalWrite(ledPinV2, HIGH);
  digitalWrite(ledPinV3, HIGH);
  digitalWrite(ledPinV4, HIGH);

  connect_MQTT(); //Llamamos a la función que realiza la conexión MQTT
}

/*-------------------------------------------------------------------------

  Char, byte, Z --> callback()

  Actúa como subscriber MQTT, recibiendo los datos del Broker
  y encendiendo o apagando LEDS 

-------------------------------------------------------------------------*/
void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageLuz;
  
  for (int i = 0; i < length; i++) {
    Serial.println((char)message[i]);
    messageLuz += (char)message[i];
  }

  if (String(topic) == lum1_topic){
    Serial.print(messageLuz);
    if (messageLuz == "true"){
      digitalWrite(ledPinV1, HIGH);
    }
    else if (messageLuz == "false") {
      digitalWrite(ledPinV1, LOW);
    }
  }else if(String(topic) == lum2_topic){
    Serial.print(messageLuz);
    if (messageLuz == "true"){
      digitalWrite(ledPinV2, HIGH);
    }
    else if (messageLuz == "false") {
      digitalWrite(ledPinV2, LOW);
    }
  }else if(String(topic) == lum3_topic){
    Serial.print(messageLuz);
    if (messageLuz == "true"){
      digitalWrite(ledPinV3, HIGH);
    }
    else if (messageLuz == "false") {
      digitalWrite(ledPinV3, LOW);
    }
  }else if(String(topic) == lum4_topic){
    Serial.print(messageLuz);
    if (messageLuz == "true"){
      digitalWrite(ledPinV4, HIGH);
    }
    else if (messageLuz == "false") {
      digitalWrite(ledPinV4, LOW);
    }
  }
}

/*-------------------------------------------------------------------------

  loop()

  Actúa como publisher MQTT, envia los datos de temperatura.

-------------------------------------------------------------------------*/
void loop() {
  client.loop();
  
  temperatura = dht.readTemperature();

  //Serials ---------------------------------------------------------------------------------------------
   if (isnan(temperatura)) {
    Serial.println("Failed to read from DHT");
  } else {
    Serial.print("Temperatura: ");
    Serial.print(temperatura);
    Serial.println(" *C");
  }
  //Fin ----------------------------------------------------------------------------------------------------

  //Publicaciones ------------------------------------------------------------------------------------------
  // MQTT can only transmit strings
  String ts="Temp: "+String((float)temperatura)+" C ";

  //Publicar temperatura ------------------------------------------------------- 
  // PUBLISH to the MQTT Broker (topic = Temperature, defined at the beginning)
  if (client.publish(temperature_topic, String(temperatura).c_str())) {
    Serial.println("Temperature sent!");
  }
  else {
    Serial.println("Temperature failed to send. Reconnecting to MQTT Broker and trying again");
    client.connect(clientID, mqtt_username, mqtt_password);
    delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
    client.publish(temperature_topic, String(temperatura).c_str());
  }
  //Fin de las Publicaciones ----------------------------------------------------------------------------------
  
  delay(5000); // print new values every 5s.
}
