#include "scheduler.h"

#include "dht_sensor.h"
#include "bmp280.h"
#include "weather_ui.h"
#include "button.h"
#include "app_state.h"
#define LED_PIN 13

unsigned long now = 0;
unsigned long lastDataTime = 0;
unsigned long lastBMP280Time = 0;
unsigned long lastDisplayTime = 0;
unsigned long lastBlinkTime = 0;
unsigned long lastButtonTime = 0;
const unsigned long measurementTime = 2000; // Alle 2 Sekunden messen (in ms)
const int N = 6; // (= 180) Anzahl der Messungen für Historie Mittelung
static float sumTemp = 0.0f; // Summe der Temperaturen für History Mittelwertberechnung 
static float sumHum = 0.0f; // Summe der Luftfeuchtigkeit für History Mittelwertberechnung 
static int historyCounter = 0; // Counter für die History

static void updateLed() {
  // Blinktask für LED, an bei Luftfeuchtigkeit über 60%
    if (now - lastBlinkTime >= 500) {
        lastBlinkTime = now;
        app.ledState = !app.ledState;
        if (app.humidity < 64.0f) {
          app.ledState = false; // LED aus, wenn Luftfeuchtigkeit unter 60%
        }
        digitalWrite(LED_PIN, app.ledState);
    }
};

static void updateSensor() {
  // Temperatur Messtask
    
    if (now - lastDataTime >= measurementTime) {
        
        lastDataTime = now;
        app.temp = dht_getTemperature();
        app.humidity = dht_getHumidity();
        app.sensorValid = true;
        historyCounter++;
        sumTemp += app.temp; // For mean value calculation (History)
        sumHum += app.humidity; // For mean value calculation (History)
    }
};

static void updateBMP280() {
  // BMP280 Druckmessung, alle 10 Sekunden
    if (now - lastBMP280Time >= 10000) {
        lastBMP280Time = now;
        float pressure = bmp280_getPressure();
        Serial.print("Pressure: ");
        Serial.print(pressure);
        Serial.println(" hPa");
        float temperature = bmp280_getTemperature();
        Serial.print(F("Temperature = "));
        Serial.print(temperature);
        Serial.println(" *C");
        float altitude = bmp280_getAltitude();
        Serial.print(F("Approx altitude = "));
        Serial.print(altitude);
        Serial.println(" m");
    }
};

static void updateMinMax() {
  // Min/Max Temperatur bestimmen
  app.minTemp = app.tempHistory[0];
  app.maxTemp = app.tempHistory[0];

  for (int i = 1; i < app.validSamples; ++i) {
    if (app.tempHistory[i] < app.minTemp) {
        app.minTemp = app.tempHistory[i];
    };
  };
  for (int i = 1; i < app.validSamples; ++i) {
    if (app.tempHistory[i] > app.maxTemp) {
      app.maxTemp = app.tempHistory[i];
    };
  };
  // Min/Max Luftfeuchtigkeit bestimmen
  app.minHumidity = app.humidityHistory[0];
  app.maxHumidity = app.humidityHistory[0];
  
  for (int i = 1; i < app.validSamples; ++i) {
    if (app.humidityHistory[i] < app.minHumidity) {
      app.minHumidity = app.humidityHistory[i];
    };
  };
  for (int i = 1; i < app.validSamples; ++i) {
    if (app.humidityHistory[i] > app.maxHumidity) {
      app.maxHumidity = app.humidityHistory[i];
    };
  };
};

static void updateHistory() {
  // Historie aktualisieren, Min/Max bestimmen
  // Temperatur Historie aktualisieren aller 180 Messungen
    if (historyCounter >= N) {

      if (app.validSamples < 128) {
      app.validSamples++;
      };
    
      // Historie aktualisieren
      app.historyIndex = (app.historyIndex + 1) % 128;
      
      app.tempHistory[app.historyIndex] = sumTemp / (float)historyCounter; // History bekommt Mittelwert der Temperatur
      app.humidityHistory[app.historyIndex] = sumHum /(float)historyCounter;
      sumTemp = 0.0f;
      sumHum = 0.0f;
      historyCounter = 0;
      
      // Update Min/Max values from history
      updateMinMax();
    };
};

static void updateDisplay() {
  // Display aktualisieren
  if (now - lastDisplayTime >= 200) {
        lastDisplayTime = now;
        if (app.sensorValid) {
            weather_show(app);
        }
    } 
};

static void updateButton() {
  // Button abfragen
  if (button_wasPressed()) {
        lastButtonTime = now;
        app.currentScreen = (uiScreen)(app.currentScreen + 1);
        if (app.currentScreen > SCREEN_GRAPH_HUM) {
            app.currentScreen = SCREEN_MAIN;
        }
    }

    // Automatischer Rücksprung zum Hauptbildschirm nach 5 Minuten Inaktivität
    if (app.currentScreen != SCREEN_MAIN && now - lastButtonTime >= 300000) { // 5 Minute Inaktivität
        app.currentScreen = SCREEN_MAIN; // Zurück zum Hauptbildschirm wechseln
    }
};

void scheduler_run() {
    now = millis();

    updateLed();
    updateSensor();
    updateBMP280();
    updateHistory();
    updateDisplay();
    updateButton();
        
}