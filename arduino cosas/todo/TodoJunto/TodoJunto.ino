#include <M5Stack.h>

// Fin de Carrera (Detecta si el usuario tiene la espalda reclinada).
const int buttonPin = 17;
int buttonState = 0;

// Sensor de Ultrasonidos (Detecta si la persona esta sentada en la silla).
const int Trigger = 2; 
const int Echo = 5; 


void setup() {

  M5.begin(true,false,false);
  Serial.begin(9600);

  pinMode(buttonPin, INPUT);

  pinMode(Trigger, OUTPUT);
  pinMode(Echo, INPUT);
  digitalWrite(Trigger, LOW);

  M5.Lcd.setCursor(5,80);
  M5.Lcd.setTextSize(2);
  M5.Lcd.print("Reclinado: ");
  M5.Lcd.drawCircle(130,88, 6, WHITE);

  M5.Lcd.setCursor(5,120);

}

void loop() {
  buttonState = digitalRead(buttonPin);
  
  if (buttonState == HIGH) {
    M5.Lcd.fillCircle(130,88, 5, WHITE);
  }
  else{
    M5.Lcd.fillCircle(130,88, 5, BLACK);  
  }

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

  if(d <= 6)
  {     
    M5.Lcd.setCursor(5,120);
    M5.Lcd.print("Bien sentado");
  }
  else if(d > 6 && d <= 12)
  {
    M5.Lcd.setCursor(5,120);
    M5.Lcd.print("Nadie sentado");
  }


  delay(500);

}
