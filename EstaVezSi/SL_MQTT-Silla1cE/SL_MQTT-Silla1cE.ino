//******************************************************************************************
//
//  Codigo MQTT - Biblioteca - Silla1_Espalda - ESP32_1.
//
//  Leds Rojos 1, Ulltrasonidos 1 + Posición de la Espalda.
//
//******************************************************************************************
#include "PubSubClient.h"
#include "WiFi.h"         

// WiFi -- Declaración
const char* ssid = "alfues62";          // SSID
const char* wifi_password = "alfues62"; // password

// MQTT -- Servidor.
const char* mqtt_server = "192.168.66.86";  // IP del MQTT broker

//MQTT -- Topicos.
const char* ultrasound1_topic = "smarlib/silla1/ultrasonidos";
const char* back_topic = "smarlib/silla1/espalda";

//MQTT -- Info del Usuario.
const char* mqtt_username = "smartlibrary"; // MQTT username.
const char* mqtt_password = "smartlibrary"; // MQTT password.

const char* clientID = "SMARTLIBRARY1";      // MQTT client ID.

//Declaración de Pines de LEDs
#define ledPinR1 17

//Declaración de Pines de Ultrasonidos1.
#define Trigger 33
#define Echo 32

//Declaración del Pin del Fin de Carrera.
#define buttonPin 34

//Declaración de variables.
int ultrasonidos1 = 0;
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
  //client.setCallback(callback);

  //PinMode -- Decaraciones del Pin del Final de Carrera.
  pinMode(buttonPin, INPUT); // pin Espalda.

  //PinMode -- Decaraciones de los pins Ultrasonidos.
  pinMode(Trigger, OUTPUT); //Salida del Ultrasonidos.
  pinMode(Echo, INPUT);     //Entrada del Ultrasonidos
  digitalWrite(Trigger, LOW);//Inicializamos el pin con 0.

  //PinMode -- Declaraciones pins LEDs.
  pinMode(ledPinR1, OUTPUT);

  connect_MQTT(); //Llamamos a la función que realiza la conexión MQTT
}

/*-------------------------------------------------------------------------

  funcUltrasonidos() --> Z

  Recoge los datos del sensor de Ultrasonidos y envia un output
  dependiendo de la distancia detectada.

-------------------------------------------------------------------------*/
int funcUltrasonidos1(){
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
    ultrasonidos1 = 1;
  }
  else if(d > 6 && d <= 8 )
  {
    Serial.print("Mal Sentado");
    ultrasonidos1 = 2;
  }
  else if (d > 8 && d <= 50){
    Serial.print("No Sentado");
    ultrasonidos1 = 3;
  }

  if (ultrasonidos1 == 3){
    digitalWrite(ledPinR1, LOW);
  }else if(ultrasonidos1 == 1 || ultrasonidos1 == 2){
    digitalWrite(ledPinR1, HIGH);
  }

  return ultrasonidos1;
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
  }else if (buttonState == LOW){
    Serial.println ("Espalda no Reclinada");
    espalda = 0;
  }

   return espalda;
}

/*-------------------------------------------------------------------------

  loop()

  Actúa como publisher MQTT, envia los datos de temperatura, 
  ultrasonidos y espalda.

  Realiza los cambios de estado de led del Ultrasonidos.

-------------------------------------------------------------------------*/
void loop() {
  client.loop();
  
  ultrasonidos1 = funcUltrasonidos1();
  espalda = funcEspalda();

  //Cambio de Estado LED según Ultrasonido --------------------------------------------------------------
  

  //Fin -------------------------------------------------------------------------------------------------

  //Serials ---------------------------------------------------------------------------------------------
  Serial.print("Estado Ultras 1: ");
  Serial.print( ultrasonidos1 );
  Serial.println( "." );

  Serial.print("Estado Int: ");
  Serial.print( espalda );
  Serial.println( "." );
  //Fin ----------------------------------------------------------------------------------------------------

  //Publicaciones ------------------------------------------------------------------------------------------
  // MQTT can only transmit strings
  String us1="Temp: "+String((int)ultrasonidos1)+" Eu ";
  String es="Temp: "+String((int)espalda)+" Ee ";


  //Publicar Ultrasonido1 --------------------------------------------------------
  // PUBLISH to the MQTT Broker (topic = Ultrasonidos1, defined at the beginning)
  if (client.publish(ultrasound1_topic, String(ultrasonidos1).c_str())) {
    Serial.println("Ultrasound1 sent!");
  }
  else {
    Serial.println("Ultrassound failed to send. Reconnecting to MQTT Broker and trying again");
    client.connect(clientID, mqtt_username, mqtt_password);
    delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
    client.publish(ultrasound1_topic, String(ultrasonidos1).c_str());
  }

  //Publicar Espalda ------------------------------------------------------------
  // PUBLISH to the MQTT Broker (topic = Espalda, defined at the beginning)
  if (client.publish(back_topic, String(espalda).c_str())) {
    Serial.println("Back sent!");
  }
  else {
    Serial.println("Back failed to send. Reconnecting to MQTT Broker and trying again");
    client.connect(clientID, mqtt_username, mqtt_password);
    delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
    client.publish(back_topic, String(espalda).c_str());
  }
  //Fin de las Publicaciones ----------------------------------------------------------------------------------
  
  delay(5000); // print new values every 5s.
}
