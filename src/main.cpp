#include <Arduino.h>
#include "dht_sensor.h"
#include "weather_ui.h"
#include "display.h"
#include "scheduler.h"
#include "button.h"
#include "bmp280.h"
#define BUTTON_PIN 3
#define LED_PIN 13

void setup() {
  Serial.begin(115200);
  dht_init();
  display_init();
  if (!bmp280_init()) {
    Serial.println("BMP280 initialization failed!");
  }
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  
  scheduler_run();
  //Serial.println(bmp280_getPressure());
}
  