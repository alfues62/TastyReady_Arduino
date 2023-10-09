#include "WiFi.h"
#include "AsyncUDP.h"

#define ssid "MOVISTAR_38E0"
#define pass "59qLEH5aHjdTi592acHq"
AsyncUDP udp;

#define rele 22

#define Trigger 33   //Pin digital 2 para el Trigger del sensor
#define Echo 32   //Pin digital 3 para el Echo del sensor

bool envioDatosDistancia = 0;

void setup()
{
  Serial.begin(115200);
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
      //Serial.print("UDP Packet Type: ");
      //Serial.print(packet.isBroadcast() ? "Broadcast" : packet.isMulticast() ? "Multicast" : "Unicast");
      //Serial.print(", From: ");
      //Serial.print(packet.remoteIP());
      //Serial.print(":");
      //Serial.print(packet.remotePort());
      //Serial.print(", To: ");
      //Serial.print(packet.localIP());
      //Serial.print(":");
      //Serial.print(packet.localPort());
      //Serial.print(", Length: ");
      //Serial.print(packet.length()); //dlzka packetu
      Serial.print(", Data: ");
      Serial.write(packet.data(), packet.length());
      Serial.println();
      
      //packet.printf("Got %u bytes of data", packet.length());
    });
  }

  // sensor distancia
  pinMode(Trigger, OUTPUT); //pin como salida
  pinMode(Echo, INPUT);  //pin como entrada
  digitalWrite(Trigger, LOW);//Inicializamos el pin con 0
  
}

void loop()
{
 
  


  // sensor distancia
  long t; //timepo que demora en llegar el eco
  long d; //distancia en centimetros

  digitalWrite(Trigger, HIGH);
  delayMicroseconds(10);          //Enviamos un pulso de 10us
  digitalWrite(Trigger, LOW);
  
  t = pulseIn(Echo, HIGH); //obtenemos el ancho del pulso
  d = t/59;             //escalamos el tiempo a una distancia en cm
  
  Serial.print("Distancia: ");
  Serial.print(d);      //Enviamos serialmente el valor de la distancia
  Serial.print("cm");
  Serial.println();
  delay(100);          //Hacemos una pausa de 100ms

  if(d <= 6)
  {
    udp.broadcast("Bien sentado");
  }
  else if(d > 6 && d <= 12)
  {
    udp.broadcast("Mal sentado");

  }
  else if(d > 12 && d < 50)
  {
    udp.broadcast("Nadie sentado");
  }

}