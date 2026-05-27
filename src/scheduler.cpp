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
//unsigned long lastHistoryTime = 0;
unsigned long lastButtonTime = 0;
const unsigned long measurementTime = 2000; // Alle 2 Sekunden messen (in ms)
const int N = 180; // Anzahl der Messungen pro Historie-Update


void scheduler_run() {
    now = millis();
    

    // Blinktask für LED, an bei Luftfeuchtigkeit über 60%
    if (now - lastBlinkTime >= 500) {
        lastBlinkTime = now;
        app.ledState = !app.ledState;
        if (app.humidity < 64.0f) {
          app.ledState = false; // LED aus, wenn Luftfeuchtigkeit unter 60%
        }
        digitalWrite(LED_PIN, app.ledState);
    }
    // Temperatur Messtask
    
    if (now - lastDataTime >= measurementTime) {
        static float temp_bar = 0.0f;
        static float hum_bar = 0.0f;
        static int n = 0; // Counter für die Trendanzeige
        static float sumTemp = 0.0f;
        static float sumHum = 0.0f;
        static int m = 0; // Counter für die History
        lastDataTime = now;
        app.temp = dht_getTemperature();
        app.humidity = dht_getHumidity();
        app.minTemp = min(app.minTemp, app.temp);
        app.maxTemp = max(app.maxTemp, app.temp);
        app.minHumidity = min(app.minHumidity, app.humidity);
        app.maxHumidity = max(app.maxHumidity, app.humidity);
        app.sensorValid = true;
        n++;
        m++;
        sumTemp += app.temp; // For mean value calculation (History)
        sumHum += app.humidity;
        temp_bar = (n-1)/((float)n) * temp_bar + 1/((float)n) * app.temp;
        hum_bar = (n-1)/((float)n) * hum_bar + 1/((float)n) * app.humidity;
        static float prevTemp_bar = 0; // Für Tendenzberechnung
        static float prevHum_bar = 0;
        
        if (n >= 30) {
            n = 0;
            app.tempStep = temp_bar - prevTemp_bar;
            prevTemp_bar = temp_bar;
            app.humidityStep = hum_bar - prevHum_bar;
            prevHum_bar = hum_bar;
            temp_bar = 0.0f;
            hum_bar = 0.0f;
        } 
        
    // Temperatur Historie aktualisieren aller 180 Messungen
    if (m >= N) {
        if (app.validSamples < 128) {
          app.validSamples++;
        }
        
        // Historie aktualisieren
        app.historyIndex = (app.historyIndex + 1) % 128;
        
        app.tempHistory[app.historyIndex] = sumTemp / (float)m; // History bekommt Mittelwert der Temperatur
        app.humidityHistory[app.historyIndex] = sumHum /(float)m;
        sumTemp = 0.0f;
        sumHum = 0.0f;
        m = 0;
    }
}

    // Anzeige aktualisieren Task
    if (now - lastDisplayTime >= 200) {
        lastDisplayTime = now;
        if (app.sensorValid) {
            weather_show(app);
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