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
unsigned long lastButtonTime = 0;
const unsigned long measurementTime = 2000; // Alle 2 Sekunden messen (in ms)
const unsigned long historyTime = 360000; // Alle 6 Minuten Historie aktualisieren (in ms)
const int N = historyTime / measurementTime; // Anzahl der Messungen pro Historie-Update

bool ledState = false;
bool sensorValid = false;
int validSamples = 0;

float temp = 0.0f;
float humidity = 0.0f;
float meanTemp = 0.0f;
float meanHum = 0.0f;
static float sumTemp = 0.0f;
static float sumHum = 0.0f;
float tempStep = 0.0f;
float humidityStep = 0.0f;
float tempHistory[128];
float humidityHistory[128];
int historyIndex = -1;

void scheduler_run() {
    now = millis();

    // Blinktask für LED, an bei Luftfeuchtigkeit über 60%
    if (now - lastBlinkTime >= 500) {
        lastBlinkTime = now;
        ledState = !ledState;
        if (app.humidity < 64.0f) {
          ledState = false; // LED aus, wenn Luftfeuchtigkeit unter 60%
        }
        digitalWrite(LED_PIN, ledState);
    }
    // Temperatur Messtask und Trendberechnung
    
    if (now - lastDataTime >= measurementTime) {
        static float temp_bar = 0.0f;
        static float hum_bar = 0.0f;
        static int n = 1; // Für die Trendberechnung alle 30 Messungen
        lastDataTime = now;
        app.temp = dht_getTemperature();
        static float prevTemp_bar = 0; // Für Tendenzberechnung
        static float prevHum_bar = 0;
        app.humidity = dht_getHumidity();
        app.sensorValid = true;

        sumTemp += app.temp; // For mean value calculation (History)
        sumHum += app.humidity;

        temp_bar = (n-1)/((float)n) * temp_bar + 1/((float)n) * app.temp;
        hum_bar = (n-1)/((float)n) * hum_bar + 1/((float)n) * app.humidity;
        n++;
        if (n % 30 == 0) {
            n = 1;
            tempStep = temp_bar - prevTemp_bar;
            prevTemp_bar = temp_bar;
            humidityStep = hum_bar - prevHum_bar;
            prevHum_bar = hum_bar;
            temp_bar = 0.0f;
            hum_bar = 0.0f;
        }
    }  
        
    // Temperatur Historie aktualisieren Task
    if (now - lastHistoryTime >= historyTime) {
        lastHistoryTime = now;

        if (app.validSamples < 128) {
          app.validSamples++;
        }
        meanTemp = sumTemp / (float)N;
        meanHum = sumHum /(float)N;
        sumTemp = sumHum = 0.0f;
        
        // Historie aktualisieren
        app.historyIndex = (app.historyIndex + 1) % 128;
        app.tempHistory[app.historyIndex] = meanTemp; // History bekommt Mittelwert der Temperatur
        app.humidityHistory[app.historyIndex] = meanHum;
    }

    // Anzeige aktualisieren Task
    if (now - lastDisplayTime >= 200) {
        lastDisplayTime = now;
        if (app.sensorValid) {
            weather_show(app.temp, app.humidity, meanTemp, meanHum);
        }
    } 

    // Button
    if (button_wasPressed()) {
        lastButtonTime = now;
        currentScreen = (uiScreen)(currentScreen + 1);
        if (currentScreen > SCREEN_GRAPH_HUM) {
            currentScreen = SCREEN_MAIN;
        }
    }

    // Automatischer Rücksprung zum Hauptbildschirm nach 5 Minuten Inaktivität
    if (currentScreen != SCREEN_MAIN && now - lastButtonTime >= 300000) { // 5 Minute Inaktivität
        currentScreen = SCREEN_MAIN; // Zurück zum Hauptbildschirm wechseln
    }
        
}