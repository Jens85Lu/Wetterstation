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

static float previousTemp = 0.0f;
char trendSymbol[2];

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
  display.drawStr(x, y + 20, "T: ");
  display.drawStr(50, y + 20, tempStr);
  display.drawStr(90, y + 20, "\xB0""C"); // Gradzeichen
  display.drawStr(120, y + 20, trendSymbol);
  display.drawStr(x, y + 40, "H: ");
  display.drawStr(50, y + 40, humidityStr);
  display.drawStr(90, y + 40, "%");
  
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
    float graphMin = 50.0f;
    float graphMax = -50.0f;
      
    display.setFont(u8g2_font_ncenB08_tr);

    for (int i = 0; i < 64; i++) {
      if (tempHistory[i] < graphMin) {
        graphMin = tempHistory[i];
      }
      if (tempHistory[i] > graphMax) {
        graphMax = tempHistory[i];
      }
    }
    graphMin = 20;
    graphMax = 24;

    float range = graphMax - graphMin;
    if (range < 0.1f) {
      range = 0.1f; // Verhindert Division durch Null
    }
 
    if (validSamples < 2) {
      display.drawStr(10, 30, "Collecting data...");
      return;
    }
    
    for (int i = 0; i < validSamples - 1; i++) {
      int index = (historyIndex - i + validSamples) % validSamples; // Ringpuffer-Index
      int prevIndex = (historyIndex - i - 1 + validSamples) % validSamples; // Vorheriger Index im Ringpuffer
      
      int x1 = 127 - 2*(i+1);
      int x2 = 127 - 2*i;
      
      int y1 = 63 - 63*((tempHistory[prevIndex] - graphMin) / (graphMax - graphMin));
      int y2 = 63 - 63*((tempHistory[index] - graphMin) / (graphMax - graphMin));
      if ((y1 > 300 || y2 > 300) && i < 4) {
        Serial.println(historyIndex);
        Serial.println("Y1: " + String(y1) + " Y2: " + String(y2));
      }

      display.drawLine(x1,y1,x2,y2);      
    }
};

void weather_show(float temp, float humidity) {
  display.clearBuffer();
  // Min/Max Temperatur bestimmen
  if (temp < minTemp) {
    minTemp = temp;
  }
  if (temp > maxTemp) {
    maxTemp = temp;
  }
  if (humidity < minHumidity) {
    minHumidity = humidity;
  }
  if (humidity > maxHumidity) {
    maxHumidity = humidity;
  }
  // Trendsymbol bestimmen
  if (temp > previousTemp) {
    trendSymbol[0] = '^'; // Aufwärtspfeil
  } else if (temp < previousTemp) {
    trendSymbol[0] = 'v'; // Abwärtspfeil
  } else {
    trendSymbol[0] = '='; // Kein Trend
  }
  trendSymbol[1] = '\0'; // Nullterminator für String
  previousTemp = temp;
  
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