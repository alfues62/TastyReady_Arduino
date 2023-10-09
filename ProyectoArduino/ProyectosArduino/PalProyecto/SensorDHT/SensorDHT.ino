#include <DHT.h>
#include <M5Stack.h>
#include <WiFi.h>
#include <SD.h>

//#define DHTPIN D5  // for ESP8266
#define DHTPIN 3  // for ESP32

#define DHTTYPE DHT11  // DHT11 or DHT22

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200); 
  dht.begin();
}
 
void loop() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
 
  // check if returns are valid and print the sensor data
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT");
  } else {
    Serial.print("Humidity: "); 
    Serial.print(humidity);
    Serial.print(" %\t");
    Serial.print("Temperature: "); 
    Serial.print(temperature);
    Serial.println(" *C");
  }

  delay(10000);
}
