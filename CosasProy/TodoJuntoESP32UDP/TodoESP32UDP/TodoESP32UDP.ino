//ESP32 UDP

#include "WiFi.h"
#include "AsyncUDP.h"

#define ssid "MiFibra-5182"
#define pass "mEF5WTPx"
AsyncUDP udp;

#define rele 22

#define Trigger 33   //Pin digital 2 para el Trigger del sensor.
#define Echo 32   //Pin digital 3 para el Echo del sensor.

#define buttonPin 35 //Pin digital para la salida del sensor de la espalda.

int buttonState = 0;
bool envioDatosDistancia = 0;

void setup()
{
  Serial.begin(9600);
  pinMode(rele, OUTPUT);

  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  if (udp.listen(1234)) {
    Serial.print("UDP Listening on IP: ");
    Serial.println(WiFi.localIP());

    udp.onPacket([](AsyncUDPPacket packet) {
      Serial.print(", Data: ");
      Serial.write(packet.data(), packet.length());
      Serial.println();
    });
  }

  // sensor distancia
  pinMode(buttonPin, INPUT); // pin Espalda.
  pinMode(Trigger, OUTPUT); //pin como salida Ultrasonidos.
  pinMode(Echo, INPUT);  //pin como entrada Ultrasonidos.
  digitalWrite(Trigger, LOW);//Inicializamos el pin con 0.

}

void loop()
{
  


  // Sensor distancia
  long t; //timepo que demora en llegar el eco
  long d; //distancia en centimetros

  digitalWrite(Trigger, HIGH);
  delayMicroseconds(10);          //Enviamos un pulso de 10us
  digitalWrite(Trigger, LOW);

  t = pulseIn(Echo, HIGH); //obtenemos el ancho del pulso
  d = t / 59;           //escalamos el tiempo a una distancia en cm

  Serial.print("Distancia: ");
  Serial.print(d);      //Enviamos serialmente el valor de la distancia
  Serial.print("cm");
  Serial.println();
  delay(100);          //Hacemos una pausa de 100ms

  if(d >= 1 && d < 4)
  {     
    udp.broadcast("Bien sentado");
  }
  else if(d > 4 && d <= 6)
  {
    udp.broadcast("Mal sentado");
  }
  else if (d > 6 && d <= 50){
    udp.broadcast("No Sentado");
  }

  /*
  if (d >= 1 && d <= 15)
  {
    udp.broadcast("Bien sentado");
  }
  else if (d > 15 && d <= 20)
  {
    udp.broadcast("Mal sentado");

  }
  else
  {
    udp.broadcast("Nadie sentado");
  }
  */

  // Switch Espalda.
  buttonState = digitalRead(buttonPin);
  
  if (buttonState == HIGH) {
    udp.broadcast("Espalda Reclinada");
  }
  else{
    udp.broadcast("Espalda sin Reclinada");
  }

}
