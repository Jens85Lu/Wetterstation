#include "scheduler.h"

#include "dht_sensor.h"
#include "bmp280.h"
#include "weather_ui.h"
#include "button.h"
#include "app_state.h"
#define LED_PIN 13
#define N 180 // Anzahl der Messungen für Historie Mittelung

unsigned long now = 0;
unsigned long lastDataTime = 0;
unsigned long lastBMP280Time = 0;
unsigned long lastDisplayTime = 0;
unsigned long lastBlinkTime = 0;
unsigned long lastButtonTime = 0;
const unsigned long measurementTime = 2000; // Alle 2 Sekunden messen (in ms)
//const int N = 6; // (= 180) Anzahl der Messungen für Historie Mittelung
static float sumTemp = 0.0f; // Summe der Temperaturen für History Mittelwertberechnung 
static float sumHum = 0.0f; // Summe der Luftfeuchtigkeit für History Mittelwertberechnung 
static float sumPressure = 0.0f; // Summe des Luftdrucks für History Mittelwertberechnung
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
    app.temp = dht_getTemperature();
    app.humidity = dht_getHumidity();
    app.pressure = bmp280_getPressure(); // Absolute Pressure in hPa
    app.pressure_seaLevel = seaLevelPressure(app.pressure, 248.0f); // Conversion to sea level pressure, da die Höhe der Sensoren über dem Meeresspiegel liegt
    app.bmpTemperature = bmp280_getTemperature();
    app.sensorValid = true;
    historyCounter++;
    sumTemp += app.temp; // For mean value calculation (History)
    sumHum += app.humidity; // For mean value calculation (History)
    sumPressure += app.pressure_seaLevel;
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
  // Min/Max Pressure bestimmen
  app.minPressure = app.pressureHistory[0];
  app.maxPressure = app.pressureHistory[0];
  
  for (int i = 1; i < app.validSamples; ++i) {
    if (app.pressureHistory[i] < app.minPressure) {
      app.minPressure = app.pressureHistory[i];
    };
  };
  for (int i = 1; i < app.validSamples; ++i) {
    if (app.pressureHistory[i] > app.maxPressure) {
      app.maxPressure = app.pressureHistory[i];
    };
  };
};

static void determineWeatherTendency() {
  // Wettertendenz bestimmen, wenn mindestens 30 gültige Messungen vorliegen
  if (app.validSamples >= 30) {
    int index30 = (app.historyIndex - 30 + 128) % 128; // Index für den Wert von vor 30 Messungen
    if (app.pressureHistory[app.historyIndex] > app.pressureHistory[index30] + 1.0f) {
      app.weatherTendency = 2; // Stark steigend
    } else if (app.pressureHistory[app.historyIndex] > app.pressureHistory[index30] + 0.5f) {
      app.weatherTendency = 1; // Steigend
    } else if (app.pressureHistory[app.historyIndex] < app.pressureHistory[index30] - 0.5f) {
      app.weatherTendency = -1; // Fallend
    } else if (app.pressureHistory[app.historyIndex] < app.pressureHistory[index30] - 1.0f) {
      app.weatherTendency = -2; // Stark fallend
    } else {
      app.weatherTendency = 0; // Stabil
    }
  }
  else {
    app.weatherTendency = 0; // Stabil, wenn nicht genügend Daten für Tendenzbestimmung vorliegen
  }
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
      
      app.tempHistory[app.historyIndex] = sumTemp / (float)historyCounter; // History bekommt Mittelwert der letzten N Messungen
      app.humidityHistory[app.historyIndex] = sumHum /(float)historyCounter;
      app.pressureHistory[app.historyIndex] = sumPressure / (float)historyCounter;
      determineWeatherTendency();

      sumTemp = 0.0f;
      sumHum = 0.0f;
      sumPressure = 0.0f;
      historyCounter = 0;
      
      // Update Min/Max values if history was updated
      updateMinMax();
    };
};

static void updateDisplay() {
  // Display aktualisieren
  if (app.sensorValid) {
      weather_show(app);
  }
};

static void updateButton() {
  // Button abfragen
  if (button_wasPressed()) {
        lastButtonTime = now;
        app.currentScreen = (uiScreen)(app.currentScreen + 1);
        if (app.currentScreen > SCREEN_TENDENCY) {
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

  if(now -lastDataTime >= measurementTime) {
      lastDataTime = now;

      updateSensor();
      updateHistory();
  }
    
    
  if (now - lastDisplayTime >= 200) {
        lastDisplayTime = now;

        updateDisplay();
  }
  
  updateButton();
  updateLed();      
}