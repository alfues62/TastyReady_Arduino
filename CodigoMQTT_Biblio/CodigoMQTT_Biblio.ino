//******************************************************************************************
//
//  Codigo MQTT - Biblioteca.
//
//  Encender y Apagar Leds, Medidas de Temperatura, Ulltrasonidos y Posición de la Espalda.
//
//  Autor: Alexander Fuset Escrivà
//  Grupo: PL1-4.
//          Alberto Alonso.
//          Aleix Llàcer.
//          Lola Pons.
//          Alexander Fuset.
//
//******************************************************************************************
#include "PubSubClient.h"
#include "WiFi.h"
#include "math.h"         

// WiFi -- Declaración
const char* ssid = "alfues62";          // SSID
const char* wifi_password = "alfues62"; // password

// MQTT -- Servidor.
const char* mqtt_server = "192.168.40.86";  // IP del MQTT broker

//MQTT -- Topicos.
const char* lum1_topic = "smarlib/silla1/led";
const char* lum2_topic = "smarlib/silla2/led";
const char* lum3_topic = "smarlib/silla3/led";
const char* lum4_topic = "smarlib/silla4/led";

const char* temperature_topic = "smarlib/biblioteca/temperatura";
const char* ultrasound_topic = "smarlib/silla1/ultrasonidos";
const char* back_topic = "smarlib/silla1/espalda";

//MQTT -- Info del Usuario.
const char* mqtt_username = "smartlibrary"; // MQTT username.
const char* mqtt_password = "smartlibrary"; // MQTT password.

const char* clientID = "SMARTLIBRARY";      // MQTT client ID.

//Declaración de Pines de LEDs
const int ledPinV1 = 16;
const int ledPinV2 = 17;
const int ledPinV3 = 25;
const int ledPinV4 = 26;
const int ledPinR1 = 36;

//Declaración de Pines de Ultrasonidos.
const int Trigger = 2;
const int Echo = 5; //El real es el pin 3 pero lo cambio para los leds

//Declaración del Pin del Fin de Carrera.
const int buttonPin = 3;

//Declaración pin temperatura.
const int TEM_PIN = 1;

//Declaración de variables.
float temperatura = 0;
int ultrasonido = 0;
int espalda = 0;
int buttonState = 0;

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
    client.subscribe(lum1_topic);
    client.subscribe(lum2_topic);
    client.subscribe(lum3_topic);
    client.subscribe(lum4_topic);
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

  connect_MQTT(); //Llamamos a la función que realiza la conexión MQTT.

  //PinMode -- Decaraciones del Pin del Final de Carrera.
  pinMode(buttonPin, INPUT); // pin Espalda.

  //PinMode -- Decaraciones de los pins Ultrasonidos.
  pinMode(Trigger, OUTPUT); //Salida del Ultrasonidos.
  pinMode(Echo, INPUT);     //Entrada del Ultrasonidos
  digitalWrite(Trigger, LOW);//Inicializamos el pin con 0.

  //PinMode -- Declaraciones pins LEDs.
  pinMode(ledPinV1, OUTPUT);
  pinMode(ledPinV2, OUTPUT);
  pinMode(ledPinV3, OUTPUT);
  pinMode(ledPinV4, OUTPUT);
  pinMode(ledPinR1, OUTPUT);

  //PinMode -- Declaracion pin temperatura.
  pinMode(TEM_PIN, OUTPUT);
  
  //Inicializa todos los LEDs verdes para que esten abiertos.
  digitalWrite(ledPinV1, HIGH);
  digitalWrite(ledPinV2, HIGH);
  digitalWrite(ledPinV3, HIGH);
  digitalWrite(ledPinV4, HIGH);

  
}

/*-------------------------------------------------------------------------

  funcUltrasonidos() --> Z

  Recoge los datos del sensor de Ultrasonidos y envia un output
  dependiendo de la distancia detectada.

-------------------------------------------------------------------------*/
int funcUltrasonidos(){
  long t; //Tiempo de demora del eco.
  long d; //Distancia en centímetros.

  digitalWrite(Trigger, HIGH);
  delayMicroseconds(10);          //Enviamos un pulso de 10us
  digitalWrite(Trigger, LOW);

  t = pulseIn(Echo, HIGH); //obtenemos el ancho del pulso
  d = t / 59;           //escalamos el tiempo a una distancia en cm

  if(d <= 6)
  {     
    Serial.print("Bien Sentado");
    ultrasonido = 1;
  }
  else if(d > 6 && d <= 8 )
  {
    Serial.print("Mal Sentado");
    ultrasonido = 2;
  }
  else if (d > 8 && d <= 50){
    Serial.print("No Sentado");
    ultrasonido = 3;
  }

  return ultrasonido;
}

/*-------------------------------------------------------------------------

  funcEspalda() --> Z

  Recoge el input del pulsador de la espalda y
  devuelve 1 o 0 segun este pulsado o no. 

-------------------------------------------------------------------------*/
int funcEspalda(){
  buttonState = digitalRead(buttonPin);

  if (buttonState == HIGH){
    Serial.println("Espalda Reclinada");
    espalda = 1;
  }else{
    Serial.println ("Espalda no Reclinada");
    espalda = 0;
  }

   return espalda;
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

  if (String(topic) == lum1_topic){ //smarlib/sillas/ilum1
    Serial.print(messageLuz);
    if (messageLuz == "false"){
      digitalWrite(ledPinV1, LOW);
    }
    else if (messageLuz == "true") {
      digitalWrite(ledPinV1, HIGH);
    }
  }else if(String(topic) == lum2_topic){ //smarlib/sillas/ilum2
    Serial.print(messageLuz);
    if (messageLuz == "false"){
      digitalWrite(ledPinV2, LOW);
    }
    else if (messageLuz == "true") {
      digitalWrite(ledPinV2, HIGH);
    }
  }else if(String(topic) == lum3_topic){ //smarlib/sillas/ilum3
    Serial.print(messageLuz);
    if (messageLuz == "false"){
      digitalWrite(ledPinV3, LOW);
    }
    else if (messageLuz == "true") {
      digitalWrite(ledPinV3, HIGH);
    }
  }else if(String(topic) == lum4_topic){ //smarlib/sillas/ilum4
    Serial.print(messageLuz);
    if (messageLuz == "false"){
      digitalWrite(ledPinV4, LOW);
    }
    else if (messageLuz == "true") {
      digitalWrite(ledPinV4, HIGH);
    }
  }
}

/*-------------------------------------------------------------------------

  loop()

  Actúa como publisher MQTT, envia los datos de temperatura, 
  ultrasonidos y espalda.

  Realiza los cambios de estado de led del Ultrasonidos.

-------------------------------------------------------------------------*/
void loop() {
  client.loop();

//  float Digital_Value = analogRead(TEM_PIN);
  temperatura = 28.5;
  ultrasonido = funcUltrasonidos();
  espalda = funcEspalda();

  //Cambio de Estado LED según Ultrasonido --------------------------------------------------------------
  if (ultrasonido == 3){
    digitalWrite(ledPinR1, LOW);
  }else if(ultrasonido == 1 || ultrasonido == 2){
    digitalWrite(ledPinR1, HIGH);
  }
  //Fin -------------------------------------------------------------------------------------------------

  //Serials ---------------------------------------------------------------------------------------------
  Serial.print("Estado Ultras: ");
  Serial.print( ultrasonido );
  Serial.println( "." );

  Serial.print("Estado Int: ");
  Serial.print( espalda );
  Serial.println( "." );
  
  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  Serial.println(" *C");
  //Fin ----------------------------------------------------------------------------------------------------

  //Publicaciones ------------------------------------------------------------------------------------------
  // MQTT can only transmit strings
  String ts="Temp: "+String((float)temperatura)+" C ";
  String us="Temp: "+String((int)ultrasonido)+" Eu ";
  String es="Temp: "+String((int)espalda)+" Ee ";

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

  //Publicar Ultrasonido --------------------------------------------------------
  // PUBLISH to the MQTT Broker (topic = Ultrasonidos, defined at the beginning)
  if (client.publish(ultrasound_topic, String(ultrasonido).c_str())) {
    Serial.println("Ultrasound sent!");
  }
  else {
    Serial.println("Ultrassound failed to send. Reconnecting to MQTT Broker and trying again");
    client.connect(clientID, mqtt_username, mqtt_password);
    delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
    client.publish(ultrasound_topic, String(ultrasonido).c_str());
  }

  //Publicar Espalda ------------------------------------------------------------
  // PUBLISH to the MQTT Broker (topic = Espalda, defined at the beginning)
  if (client.publish(back_topic, String(espalda).c_str())) {
    Serial.println("Back sent!");
  }
  else {
    Serial.println("Ultrassound failed to send. Reconnecting to MQTT Broker and trying again");
    client.connect(clientID, mqtt_username, mqtt_password);
    delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
    client.publish(back_topic, String(espalda).c_str());
  }
  //Fin de las Publicaciones ----------------------------------------------------------------------------------
  
  delay(5000); // print new values every 5s.
}
