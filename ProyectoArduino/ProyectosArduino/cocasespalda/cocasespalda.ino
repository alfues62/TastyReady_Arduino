#define buttonPin 34

int espalda = 0;
int buttonState = 0;

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT); // pin Espalda.


}

void loop() {
  buttonState = digitalRead(buttonPin);

  if (buttonState == HIGH){
    Serial.println("Espalda Reclinada");
    espalda = 1;
  }else if (buttonState == LOW){
    Serial.println ("Espalda no Reclinada");
    espalda = 0;
  }

}
