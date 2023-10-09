#define LED 2
#define PUSH 3 

int pushvalue = 0;

void setup(){
  pinMode(LED, OUTPUT);
  pinMode(PUSH, INPUT);
    
}

void loop(){

    pushvalue = digitalRead(PUSH);

    if(pushvalue == 1){

       digitalWrite(LED, HIGH); 
  
    }
    else{

       digitalWrite(LED, LOW); 
    }

}
