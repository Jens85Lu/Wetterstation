#include "scheduler.h"
#include "dht_sensor.h"
#include "weather_ui.h"
#include "button.h"
#include "app_state.h"
#define LED_PIN 13

unsigned long now = 0;
unsigned long lastDataTime = 0;
unsigned long lastDisplayTime = 0;
unsigned long lastBlinkTime = 0;
unsigned long lastHistoryTime = 0;
bool ledState = false;
bool sensorValid = false;
int validSamples = 0;

float temp = 0.0f;
float humidity = 0.0f;
float tempHistory[64];
int historyIndex = -1;

void scheduler_run() {
    now = millis();
    // Blinktask
    if (now - lastBlinkTime >= 500) {
        lastBlinkTime = now;
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState);
    }
    // Temperatur Messtask
    
    if (now - lastDataTime >= 2000) {
        lastDataTime = now;
        temp = dht_getTemperature();
        humidity = dht_getHumidity();
        sensorValid = true;
    }
        
    // Historie aktualisieren Task
    if (now - lastHistoryTime >= 20000) {
        lastHistoryTime = now;

        if (validSamples < 64) {
          validSamples++;
        }
        // Historie aktualisieren
        historyIndex = (historyIndex + 1) % 64;

        tempHistory[historyIndex] = temp;
    }

    // Anzeige aktualisieren Task
    if (now - lastDisplayTime >= 200) {
        lastDisplayTime = now;
        if (sensorValid) {
            weather_show(temp, humidity);
        }
    } 
    
    // Button
    if (button_wasPressed()) {
        currentScreen = (uiScreen)(currentScreen + 1);
        if (currentScreen > SCREEN_GRAPH) {
            currentScreen = SCREEN_MAIN;
        }
    }
}