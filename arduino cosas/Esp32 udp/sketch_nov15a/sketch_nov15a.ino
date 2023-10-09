int LED = 6;
#define sw_1 7

void setup(){

    pinMode(LED, OUTPUT);

}

void loop(){

    if(digitalRead(sw_1)==1){

        digitalWrite(LED, HIGH);

    }

    else{

        digitalWrite(LED, LOW);
    
    }

}
