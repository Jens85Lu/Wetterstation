// Anzeigelogik

#include "weather_ui.h"
#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include "app_state.h"
#include "scheduler.h"

extern U8G2_SH1106_128X64_NONAME_F_HW_I2C display;
static bool lastEdge = false; // For Screensaver animation
static int x = 0; // movement in x direction
static int y = 0; // movement in y direction

static float minTemp = 999.0f;
static float maxTemp = -999.0f;
static float minHumidity = 999.0f;
static float maxHumidity = -999.0f;

static char tempTrendSymbol[2];
static char humidityTrendSymbol[2];

static void makeAnimation() {
  // Einfache Animation: Text bewegt sich von rechts nach links und zurück
  if (x > 20) {
    lastEdge = 1;
  } else if (x < 0) {
    lastEdge = 0;
  }
  if (lastEdge == 1) {
    x--;
  } else {
    x++;
  }
  if (y > 20) {
    lastEdge = 1;
  } else if (y < 0) {
    lastEdge = 0;
  }
  if (lastEdge == 1) {
    y--;
  } else {
    y++;
  }
}

static void drawMainScreen(float temp, float humidity){
  char tempStr[16];
  char humidityStr[16];
  display.setFont(u8g2_font_crox4hb_tf);
  dtostrf(temp, 4, 1, tempStr);
  dtostrf(humidity, 4, 1, humidityStr);
  // Temperatur-Trendsymbol bestimmen
  if (tempStep > 0.2f) {  
    tempTrendSymbol[0] = '^'; // Aufwärtspfeil
  } else if (tempStep < -0.2f) {
    tempTrendSymbol[0] = 'v'; // Abwärtspfeil
  } else {
    tempTrendSymbol[0] = '='; // Kein Trend
  }
  tempTrendSymbol[1] = '\0'; // Nullterminator für String

  // Luftfeuchtigkeits-Trendsymbol bestimmen
  if (humidityStep > 0.4f) {
    humidityTrendSymbol[0] = '^'; // Aufwärtspfeil
  } else if (humidityStep < -0.4f) {
    humidityTrendSymbol[0] = 'v'; // Abwärtspfeil
  } else {
    humidityTrendSymbol[0] = '='; // Kein Trend
  }
  humidityTrendSymbol[1] = '\0'; // Nullterminator für String
  display.drawStr(x, y + 20, "T: ");
  display.drawStr(50, y + 20, tempStr);
  display.drawStr(90, y + 20, "\xB0""C"); // Gradzeichen
  display.drawStr(110, y + 20, tempTrendSymbol);
  display.drawStr(x, y + 40, "H: ");
  display.drawStr(50, y + 40, humidityStr);
  display.drawStr(90, y + 40, "%");
  display.drawStr(110, y + 40, humidityTrendSymbol);
  
  makeAnimation();
}

static void drawMinMaxScreen() {
  
    char minTempStr[16];
    char maxTempStr[16];
    char minHumidityStr[16];
    char maxHumidityStr[16];
    dtostrf(minTemp, 4, 1, minTempStr);
    dtostrf(maxTemp, 4, 1, maxTempStr);
    dtostrf(minHumidity, 4, 1, minHumidityStr);
    dtostrf(maxHumidity, 4, 1, maxHumidityStr); 

    display.setFont(u8g2_font_ncenB08_tr);

    display.drawStr(0, 15, "Min Temp:");
    display.drawStr(80, 15, minTempStr);

    display.drawStr(0, 30, "Max Temp:");
    display.drawStr(80, 30, maxTempStr);

    display.drawStr(0, 45, "Min Humi:");
    display.drawStr(80, 45, minHumidityStr);

    display.drawStr(0, 60, "Max Humi:");
    display.drawStr(80, 60, maxHumidityStr);
}

static void drawGraphScreen() {
    float graphMin = minTemp;
    float graphMax = maxTemp;
      
    display.setFont(u8g2_font_ncenB08_tr);

    // Mindestrange von 1 Grad sicherstellen
    float range = graphMax - graphMin;
    if (range < 1.0f) {
      graphMin = graphMin - (1.0f - range) / 2.0f;
      graphMax = graphMax + (1.0f - range) / 2.0f;
    }
    // Es werden zwei Datenpunkte gebraucht, um eine Linie zu zeichnen
    if (validSamples < 2) {
      display.drawStr(10, 30, "Collecting data...");
      return;
    }
    // Historie als Graph zeichnen, neueste Daten rechts
    for (int i = 1; i < validSamples; i++) {
      int index = (historyIndex - (i-1) + validSamples) % validSamples; // Ringpuffer-Index
      int prevIndex = (historyIndex - i + validSamples) % validSamples; // Vorheriger Index im Ringpuffer
      
      int x1 = 128 - i;
      int x2 = 128 - (i-1);
      
      int y1 = 63 - 49*((tempHistory[prevIndex] - graphMin) / (graphMax - graphMin));
      int y2 = 63 - 49*((tempHistory[index] - graphMin) / (graphMax - graphMin));

      display.drawLine(x1,y1,x2,y2);      
    }
    // Achsen zeichnen und Beschriftung hinzufügen
    display.drawLine(0, 14, 0, 63); // Y-Achse
    display.drawLine(0, 63, 127, 63); // X-Achse
    String header = String(graphMin, 1) + " < Temp(10h) < " + String(graphMax, 1);
    display.drawStr(0, 12, header.c_str());
  }

void weather_show(float temp, float humidity) {
  display.clearBuffer();

  // Min/Max Temperatur bestimmen
  minTemp = tempHistory[0];
  maxTemp = tempHistory[0];
  for (int i = 1; i < validSamples; ++i) {
    if (tempHistory[i] < minTemp) {
        minTemp = tempHistory[i];
    }
  }
  for (int i = 1; i < validSamples; ++i) {
    if (tempHistory[i] > maxTemp) {
        maxTemp = tempHistory[i];
    }
  }
  // Min/Max Luftfeuchtigkeit bestimmen
  minHumidity = humidityHistory[0];
  maxHumidity = humidityHistory[0];
  for (int i = 1; i < validSamples; ++i) {
    if (humidityHistory[i] < minHumidity) {
        minHumidity = humidityHistory[i];
    }
  }
  for (int i = 1; i < validSamples; ++i) {
    if (humidityHistory[i] > maxHumidity) {
        maxHumidity = humidityHistory[i];
    }
  }

  // State Machine für den Bildschirm
  switch(currentScreen) {
    case SCREEN_MAIN:
        drawMainScreen(temp, humidity);
        break;
    case SCREEN_MINMAX:
        drawMinMaxScreen();
        break;
    case SCREEN_GRAPH:
        drawGraphScreen();
        break;
}
  display.sendBuffer();
  
}