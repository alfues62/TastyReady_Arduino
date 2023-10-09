//******************************************************************************************
//
//  Codigo MQTT - Biblioteca - Silla4 - ESP32_4.
//
//  Leds Rojos 4, Ulltrasonidos 4.
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
const char* ultrasound4_topic = "smarlib/silla4/ultrasonidos";

//MQTT -- Info del Usuario.
const char* mqtt_username = "smartlibrary"; // MQTT username.
const char* mqtt_password = "smartlibrary"; // MQTT password.

const char* clientID = "SMARTLIBRARY4";      // MQTT client ID.

//Declaración de Pines de LEDs
#define ledPinR4 18

//Declaración de Pines de Ultrasonidos1.
#define Trigger 33
#define Echo 32

//Declaración de variables.
int ultrasonidos4 = 0;

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

  //PinMode -- Decaraciones de los pins Ultrasonidos3.
  pinMode(Trigger, OUTPUT); //Salida del Ultrasonidos.
  pinMode(Echo, INPUT);     //Entrada del Ultrasonidos
  digitalWrite(Trigger, LOW);//Inicializamos el pin con 0.

  //PinMode -- Declaraciones pins LEDs.
  pinMode(ledPinR4, OUTPUT);

  connect_MQTT(); //Llamamos a la función que realiza la conexión MQTT
}

/*-------------------------------------------------------------------------

  funcUltrasonidos() --> Z

  Recoge los datos del sensor de Ultrasonidos y envia un output
  dependiendo de la distancia detectada.

-------------------------------------------------------------------------*/
int funcUltrasonidos4(){
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
    ultrasonidos4 = 1;
  }
  else if(d > 6 && d <= 8 )
  {
    Serial.print("Mal Sentado");
    ultrasonidos4 = 2;
  }
  else if (d > 8 && d <= 50){
    Serial.print("No Sentado");
    ultrasonidos4 = 3;
  }

  return ultrasonidos4;
}

/*-------------------------------------------------------------------------

  loop()

  Actúa como publisher MQTT, envia los datos de temperatura, 
  ultrasonidos y espalda.

  Realiza los cambios de estado de led del Ultrasonidos.

-------------------------------------------------------------------------*/
void loop() {
  client.loop();
  
  ultrasonidos4 = funcUltrasonidos4();
  
  //Cambio de Estado LED según Ultrasonido --------------------------------------------------------------
  if (ultrasonidos4 == 3){
    digitalWrite(ledPinR4, LOW);
  }else if(ultrasonidos4 == 1 || ultrasonidos4 == 2){
    digitalWrite(ledPinR4, HIGH);
  }
  //Fin -------------------------------------------------------------------------------------------------

  //Serials ---------------------------------------------------------------------------------------------
  Serial.print("Estado Ultras 4: ");
  Serial.print( ultrasonidos4 );
  Serial.println( "." );
  //Fin ----------------------------------------------------------------------------------------------------

  //Publicaciones ------------------------------------------------------------------------------------------
  // MQTT can only transmit strings
  String us4="Temp: "+String((int)ultrasonidos4)+" Eu ";

  //Publicar Ultrasonido4 --------------------------------------------------------
  // PUBLISH to the MQTT Broker (topic = ultrasonidos4, defined at the beginning)
  if (client.publish(ultrasound4_topic, String(ultrasonidos4).c_str())) {
    Serial.println("Ultrasound4 sent!");
  }
  else {
    Serial.println("Ultrassound failed to send. Reconnecting to MQTT Broker and trying again");
    client.connect(clientID, mqtt_username, mqtt_password);
    delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
    client.publish(ultrasound4_topic, String(ultrasonidos4).c_str());
  }
  //Fin de las Publicaciones ----------------------------------------------------------------------------------
  
  delay(5000); // print new values every 5s.
}
