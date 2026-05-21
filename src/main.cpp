#include <Arduino.h>
#include "dht_sensor.h"
#include "weather_ui.h"
#include "display.h"
#include "scheduler.h"
#include "button.h"
#define BUTTON_PIN 3
#define LED_PIN 13

void setup() {
  Serial.begin(115200);
  dht_init();
  display_init();
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  
  scheduler_run();
}
  