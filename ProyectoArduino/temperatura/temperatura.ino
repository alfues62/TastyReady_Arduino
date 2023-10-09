#include <M5Stack.h>
#include <math.h>
#define TEM_PIN 2
void setup() {
  Serial.begin(9600);
  pinMode(TEM_PIN, OUTPUT);
}


void loop() {
  float Digital_Value = analogRead(TEM_PIN); //读取串联电阻上的电压值（数字量）
 //float tem_v=map(Digital_Value,0,2048,0,100);
// float Voltage_Value = (tem_v / 100) * 3.3; //换算成模拟量的电压值 /Calcular el voltaje
 //float Rt_Value = 100*(3.3 - Voltage_Value) / Voltage_Value; //计算出热敏电阻的阻值 /Calcular resistencia
  //计算所感知的温度并发送
   Serial.print((Digital_Value-500)/29);
   Serial.println("ºC");
   //Serial.println(tem_v);
   //Serial.println(Voltage_Value);
 //Serial.println(Rt_Value);
 
  delay(3000);   //三秒刷新一次
}
