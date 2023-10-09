// M5Stack UDP

#include "WiFi.h"
#include "AsyncUDP.h"
#include <M5Stack.h>
const char* ssid = "MiFibra-5182";
const char* pass = "mEF5WTPx";
const int rele = 23;
const int lampara = 22;
// propiedades PWM
const int frecuencia = 5000;
const int canal = 0;
const int resolucion = 8;
int dutyCycle = 0;
int contadorMostrarLuz = 90;

AsyncUDP udp;

void setup()
{
  M5.begin(true,false,false);
  Serial.begin(9600);
  pinMode(rele, OUTPUT);
 
  // configuramos la funcionalidad PWM y asociamos el canal al GPIO
  ledcSetup(canal, frecuencia, resolucion);
  ledcAttachPin(lampara, canal);
  // ---------------------------------
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
      
      Serial.write(packet.data(), packet.length());
      //Serial.println();
      
      String datosUDP = (const char*)packet.data();
      String textoMostrarEnM5Ultra = "";
      String textoMostrarEnM5Esp = "";

      if (datosUDP == "Espalda Reclinada"){
        textoMostrarEnM5Esp = "Reclinada ";
      }
      else{
        textoMostrarEnM5Esp = "No Reclinada ";
      }

      if (datosUDP == "Bien sentado") 
      {
        textoMostrarEnM5Ultra = "Bien sentado ";
      } 
      else if (datosUDP == "Mal sentado") 
      {
        textoMostrarEnM5Ultra = "Mal sentado  ";
      }
      else if(datosUDP == "Nadie sentado")
      {
        textoMostrarEnM5Ultra = "Nadie sentado";
      }

        M5.Lcd.setCursor(60,60);
        M5.Lcd.setTextSize(2);
        M5.Lcd.println(textoMostrarEnM5Ultra);
      
      //packet.printf("Got %u bytes of data", packet.length());
    });

    M5.Lcd.setCursor(5,8);
  M5.Lcd.print("Iluminacion ");
  M5.Lcd.drawLine(79,4,320,4, WHITE);
  M5.Lcd.drawLine(79,16,320,16, WHITE);
  M5.Lcd.drawLine(319, 4, 319, 16, WHITE);
  M5.Lcd.drawLine(79,4,79,16, WHITE);
  }
  
    

}

void loop()
{
  
  M5.update();
  
 


  if(M5.BtnC.isPressed() && dutyCycle < 25500){
// Incrementamos el brillo
  dutyCycle++;

  
  contadorMostrarLuz += 1;
  M5.Lcd.drawLine((contadorMostrarLuz/100)+80, 5, (contadorMostrarLuz/100)+80, 15, WHITE);
  
 
  
  }
  if(M5.BtnA.isPressed() && dutyCycle > 0){
// Decrementamos el brillo
   dutyCycle--;

    if(dutyCycle < 23800)
    {
     M5.Lcd.drawLine((contadorMostrarLuz/100)+80, 5, (contadorMostrarLuz/100)+80, 15, BLACK);
    }
     contadorMostrarLuz -= 1;
   
  }
  int algo = dutyCycle / 100;
  ledcWrite(canal, algo);
  
  

  

  
}
