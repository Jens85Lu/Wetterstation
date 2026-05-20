#include <Arduino.h>
#include "dht_sensor.h"
#include "weather_ui.h"
#include "display.h"
#include "scheduler.h"

void setup() {
  Serial.begin(115200);
  dht_init();
  display_init();

}

void loop() {
  
  scheduler_run();  
  
}
  