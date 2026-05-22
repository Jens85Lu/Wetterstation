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
float tempStep = 0.0f;
float humidityStep = 0.0f;
float tempHistory[120];
float humidityHistory[120];
int historyIndex = -1;

void scheduler_run() {
    now = millis();
    // Blinktask für LED, an bei Luftfeuchtigkeit über 60%
    if (now - lastBlinkTime >= 500) {
        lastBlinkTime = now;
        ledState = !ledState;
        if (humidity < 60.0f) {
          ledState = false; // LED aus, wenn Luftfeuchtigkeit unter 60%
        }
        digitalWrite(LED_PIN, ledState);
    }
    // Temperatur Messtask und Trendberechnung
    
    if (now - lastDataTime >= 2000) {
        static int counter = -1;
        counter++; // für Trendberechnung alle 5 Messungen
        lastDataTime = now;
        temp = dht_getTemperature();
        static float savedTemperature = temp; // Für Tendenzberechnung
        humidity = dht_getHumidity();
        static float savedHumidity = humidity; // Für Tendenzberechnung
        sensorValid = true;
        if (counter % 5 == 0) {
          tempStep = temp - savedTemperature;
          savedTemperature = temp;
        }
        if (counter % 5 == 0) {
          humidityStep = humidity - savedHumidity;
          savedHumidity = humidity;
        }
    }  
        
    // Temperatur Historie aktualisieren Task
    if (now - lastHistoryTime >= 300000) { // Alle 5 Minuten
        lastHistoryTime = now;

        if (validSamples < 120) {
          validSamples++;
        }
        // Historie aktualisieren
        historyIndex = (historyIndex + 1) % 120;

        tempHistory[historyIndex] = temp;
        humidityHistory[historyIndex] = humidity;
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