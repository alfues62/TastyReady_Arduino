//******************************************************************************************
//
//  Codigo MQTT - Biblioteca - Silla2 - ESP32_2.
//
//  Led Rojo 2, Ulltrasonidos 2.
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
const char* ultrasound2_topic = "smarlib/silla2/ultrasonidos";

//MQTT -- Info del Usuario.
const char* mqtt_username = "smartlibrary"; // MQTT username.
const char* mqtt_password = "smartlibrary"; // MQTT password.

const char* clientID = "SMARTLIBRARY2";      // MQTT client ID.

//Declaración de Pines de LEDs
#define ledPinR2 18

//Declaración de Pines de ultrasonidos2.
#define Trigger 33
#define Echo 32

//Declaración de variables.
int ultrasonidos2 = 0;

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

  //PinMode -- Decaraciones de los pins Ultrasonidos2.
  pinMode(Trigger, OUTPUT); //Salida del Ultrasonidos.
  pinMode(Echo, INPUT);     //Entrada del Ultrasonidos
  digitalWrite(Trigger, LOW);//Inicializamos el pin con 0.

  //PinMode -- Declaraciones pins LEDs.
  pinMode(ledPinR2, OUTPUT);

  connect_MQTT(); //Llamamos a la función que realiza la conexión MQTT
}

/*-------------------------------------------------------------------------

  funcUltrasonidos() --> Z

  Recoge los datos del sensor de Ultrasonidos y envia un output
  dependiendo de la distancia detectada.

-------------------------------------------------------------------------*/
int funcUltrasonidos2(){
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
    ultrasonidos2 = 1;
  }
  else if(d > 6 && d <= 8 )
  {
    Serial.print("Mal Sentado");
    ultrasonidos2 = 2;
  }
  else if (d > 8 && d <= 50){
    Serial.print("No Sentado");
    ultrasonidos2 = 3;
  }

  return ultrasonidos2;
}

/*-------------------------------------------------------------------------

  loop()

  Actúa como publisher MQTT, envia los datos de temperatura, 
  ultrasonidos y espalda.

  Realiza los cambios de estado de led del Ultrasonidos.

-------------------------------------------------------------------------*/
void loop() {
  client.loop();
  
  ultrasonidos2 = funcUltrasonidos2();
  
  //Cambio de Estado LED según Ultrasonido --------------------------------------------------------------
  if (ultrasonidos2 == 3){
    digitalWrite(ledPinR2, LOW);
  }else if(ultrasonidos2 == 1 || ultrasonidos2 == 2){
    digitalWrite(ledPinR2, HIGH);
  }
  //Fin -------------------------------------------------------------------------------------------------

  //Serials ---------------------------------------------------------------------------------------------
  Serial.print("Estado Ultras 2: ");
  Serial.print( ultrasonidos2 );
  Serial.println( "." );
  //Fin ----------------------------------------------------------------------------------------------------

  //Publicaciones ------------------------------------------------------------------------------------------
  // MQTT can only transmit strings
  String us2="Temp: "+String((int)ultrasonidos2)+" Eu ";

  //Publicar Ultrasonido2 --------------------------------------------------------
  // PUBLISH to the MQTT Broker (topic = ultrasonidos2, defined at the beginning)
  if (client.publish(ultrasound2_topic, String(ultrasonidos2).c_str())) {
    Serial.println("Ultrasound2 sent!");
  }
  else {
    Serial.println("Ultrassound2 failed to send. Reconnecting to MQTT Broker and trying again");
    client.connect(clientID, mqtt_username, mqtt_password);
    delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
    client.publish(ultrasound2_topic, String(ultrasonidos2).c_str());
  }
  //Fin de las Publicaciones ----------------------------------------------------------------------------------
  
  delay(5000); // print new values every 5s.
}
