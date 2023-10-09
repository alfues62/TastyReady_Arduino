#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
//#include <AsyncUDP.h>
#include <M5Stack.h>

// Preparación conexión MQTT.
WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[50];
int value = 0;

//SSID-Password.
const char* ssid = "MobilAlex";
const char* password = "contrasenya";

//Direccion MQTT Broker IP Address.
const char* mqtt_server = "192.168.40.86";

//Declaración de Pines de LEDs
const int ledPinV1 = 25;
const int ledPinV2 = 26;
const int ledPinV3 = 35;
const int ledPinV4 = 36;
const int ledPinR1 = 2;

//Declaración de variables.
float temperatura = 0;
  //Variables del calculo de la temperatura.
  const float b = 0.79;
  const float m = 0.035;
int ultrasonido = 0;
int espalda = 0;

//Preparación conexión UDP.
//AsyncUDP udp;

/*---------------------------------------------------------

setup() --> Realiza la configuración inicial.

---------------------------------------------------------*/
void setup() {

  M5.begin(true,false,false);
  Serial.begin(9600);
  setup_wifi();

  client.setServer(mqtt_server, 1880);
  client.setCallback(callback);

  //PinMode -- Declaraciones pins LEDs.
  pinMode(ledPinV1, OUTPUT);
  pinMode(ledPinV2, OUTPUT);
  pinMode(ledPinV3, OUTPUT);
  pinMode(ledPinV4, OUTPUT);
  pinMode(ledPinR1, OUTPUT);

  //Inicializa todos los LEDs verdes para que esten abiertos.
  digitalWrite(ledPinV1, HIGH);
  digitalWrite(ledPinV2, HIGH);
  digitalWrite(ledPinV3, HIGH);
  digitalWrite(ledPinV4, HIGH);

  //connex_udp(); // Conectando con ESP32 mediante UDP.
  

}
/*---------------------------------------------------------

setup_wifi() --> Realiza la conexión con WiFi.

---------------------------------------------------------*/
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
/*---------------------------------------------------------

connex_udp() --> Realiza la conexión con la placa ESP32 mediante UDP ( Recibe Datos ).

---------------------------------------------------------*/
/*void connex_udp(){
  if (udp.listen(1234)) {
    Serial.print("UDP Listening on IP: ");
    Serial.println(WiFi.localIP());
   
    udp.onPacket([](AsyncUDPPacket packet) {
      
      Serial.write(packet.data(), packet.length());
      
      String datosUDP = (const char*)packet.data();
      String textoMostrarEnM5Ultra = "";
      String textoMostrarEnM5Esp = "";

      if (datosUDP == "No Sentado") 
      {
        textoMostrarEnM5Ultra = "No Sentado";
        digitalWrite(ledPinR1, LOW);
        ultrasonido = 0;
      } 
      else if (datosUDP == "Mal sentado") 
      {
        textoMostrarEnM5Ultra = "Mal sentado  ";
        digitalWrite(ledPinR1, HIGH);
        ultrasonido = 1;
      }
      else if(datosUDP == "Bien sentado")
      {
        textoMostrarEnM5Ultra = "Bien sentado ";
        digitalWrite(ledPinR1, HIGH);
        ultrasonido = 2;
      }

        M5.Lcd.setCursor(60,90);
        M5.Lcd.setTextSize(2);
        M5.Lcd.println(textoMostrarEnM5Ultra);

      if (datosUDP == "Espalda Reclinada"){
        textoMostrarEnM5Esp = "Espalda Reclinada ";
        espalda = 1;
      }
      else if(datosUDP == "Espalda sin Reclinada"){
        textoMostrarEnM5Esp = "Sin Reclinar";
        espalda = 0;
      }

      M5.Lcd.setCursor(60,60);
      M5.Lcd.setTextSize(2);
      M5.Lcd.println(textoMostrarEnM5Esp);
    });
  }
  
}*/
/*---------------------------------------------------------

callback() --> MQTT -- subscirber( recibe datos ).

---------------------------------------------------------*/
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
    if(messageTemp == "on"){
      Serial.println("on");
      digitalWrite(ledPinV1, HIGH);
    }else if(messageTemp == "off"){
      Serial.println("off");
      digitalWrite(ledPinV1, LOW);
    }
  }
  //Output (Topic: esp32/output2)
  if (String(topic) == "esp32/output2")
  {
    Serial.print("Cambio de salida: ");
    if(messageTemp == "on"){
      Serial.println("on");
      digitalWrite(ledPinV2, HIGH);
    }else if(messageTemp == "off"){
      Serial.println("off");
      digitalWrite(ledPinV2, LOW);
    }
  }
  //Output (Topic: esp32/output3)
  if (String(topic) == "esp32/output3")
  {
    Serial.print("Cambio de salida: ");
    if(messageTemp == "on"){
      Serial.println("on");
      digitalWrite(ledPinV3, HIGH);
    }else if(messageTemp == "off"){
      Serial.println("off");
      digitalWrite(ledPinV3, LOW);
    }
  }
  //Output (Topic: esp32/output4)
  if (String(topic) == "esp32/output4")
  {
    Serial.print("Cambio de salida: ");
    if(messageTemp == "on"){
      Serial.println("on");
      digitalWrite(ledPinV4, HIGH);
    }else if(messageTemp == "off"){
      Serial.println("off");
      digitalWrite(ledPinV4, LOW);
    }
  }
 
}
/*---------------------------------------------------------

reconnect() --> Reconecta en caso de error.

---------------------------------------------------------*/
void reconnect(){
  
  while(!client.connected()){
    Serial.println("Intentando reconexion MQTT ...");

    if(client.connect("ESP32Client")){
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
/*---------------------------------------------------------

funcTemperatura() --> Función que calcula la temperatura. 

---------------------------------------------------------*/
/*float funcTemperatura(){

//--->  Variables del programa 
  int16_t adc2;
  float voltios, temperatura;
  
  adc2=ads1115.readADC_SingleEnded(2);  //Asignar el valor leido por el micro a una variable.
  
  voltios = 4.096*adc2/(pow(2,15)-1); 
  temperatura = (voltios - b)/m;

  return temperatura;
}*/
/*---------------------------------------------------------

loop() --> MQTT -- publisher ( envia datos ).

---------------------------------------------------------*/
void loop() {

  M5.update();

  
  if(!client.connected()){
    reconnect();
  }
  //client.loop();

  long now = millis();
  if(now - lastMsg > 100){
    lastMsg = now;

    char espString[2];
    dtostrf(espalda, 1, 2, espString);
    client.publish("esp32/back", espString);

    char ultraString[2];
    dtostrf(ultrasonido, 1, 2, ultraString);
    client.publish("esp32/ultrasounds", ultraString);

    delay(300);

    /*temperatura = funcTemperatura();
    char tempString[8];
    dtostrf(temperatura, 1, 2, tempString);
    Serial.println("Temperatura");
    client.publish("esp32/temperature", tempString);
    */
  }

  client.loop();

}
