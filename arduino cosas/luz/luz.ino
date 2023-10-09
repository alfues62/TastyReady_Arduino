#include <M5Stack.h>

int ledPin = 21;

int pushvalue = 0;

void setup() {
  pinMode(ledPin, OUTPUT); 
}

void loop() {

  if(M5.BtnB.isPressed() == true){
    pushvalue = 1;
  }

  if(pushvalue = 1){
    digitalWrite(ledPin, HIGH);
  }else{
    digitalWrite(ledPin, LOW);
  }
  
}
