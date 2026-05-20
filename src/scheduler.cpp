#include "scheduler.h"
#include "dht_sensor.h"
#include "weather_ui.h"

unsigned long now = 0;
unsigned long lastDataTime = 0;
unsigned long lastDisplayTime = 0;
unsigned long lastBlinkTime = 0;
bool ledState = false;

float temp = 0.0f;
float humidity = 0.0f;

void scheduler_run() {
    now = millis();
    // Blinktask
    if (now - lastBlinkTime >= 500) {
        lastBlinkTime = now;
        ledState = !ledState;
        digitalWrite(13, ledState);
    }
    // Temperaturtask
    
    if (now - lastDataTime >= 2000) {
        lastDataTime = now;
        temp = dht_getTemperature();
        humidity = dht_getHumidity();
    }
    // Anzeigetask
    if (now - lastDisplayTime >= 200) {
        lastDisplayTime = now;
        weather_show(temp, humidity);
    }
}