#include <Arduino.h>
#include "dht_sensor.h"
#include "weather_ui.h"
#include "display.h"

void setup() {
  Serial.begin(115200);
  dht_init();
  display_init();

}

void loop() {
  
  float temp = dht_getTemperature();
  float humidity = dht_getHumidity();
  if (isnan(temp) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    display_showText("Sensor error!");
  } else {
    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.print(" °C, Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");
    
    weather_show(temp, humidity);
  }
  delay(2000); // Update every 2 seconds
  Serial.println(sizeof(float));
Serial.println(sizeof(double));
  
}
  