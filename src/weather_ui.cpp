// Anzeigelogik

#include "weather_ui.h"

#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include "app_state.h"
#include "scheduler.h"

extern U8G2_SH1106_128X64_NONAME_F_HW_I2C display;
// For Screensaver animation
static bool lastEdge = false; 
static int x = 0; // movement in x direction
static int y = 0; // movement in y direction
// Trendsymbole
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

static void drawMainScreen(AppData& data){
  char tempStr[16];
  char humidityStr[16];
  display.setFont(u8g2_font_crox4hb_tf);
  dtostrf(data.temp, 4, 1, tempStr);
  dtostrf(data.humidity, 4, 1, humidityStr);
  // Temperatur-Trendsymbol bestimmen
  if (data.tempStep > 0.037f) {  
    tempTrendSymbol[0] = '^'; // Aufwärtspfeil
  } else if (data.tempStep < -0.037f) {
    tempTrendSymbol[0] = 'v'; // Abwärtspfeil
  } else {
    tempTrendSymbol[0] = '='; // Kein Trend
  }
  tempTrendSymbol[1] = '\0'; // Nullterminator für String

  // Luftfeuchtigkeits-Trendsymbol bestimmen
  if (data.humidityStep > 0.1f) {
    humidityTrendSymbol[0] = '^'; // Aufwärtspfeil
  } else if (data.humidityStep < -0.1f) {
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

static void drawMinMaxScreen(AppData& data) {
  
    char minTempStr[16];
    char maxTempStr[16];
    char minHumidityStr[16];
    char maxHumidityStr[16];
    dtostrf(data.minTemp, 4, 1, minTempStr);
    dtostrf(data.maxTemp, 4, 1, maxTempStr);
    dtostrf(data.minHumidity, 4, 1, minHumidityStr);
    dtostrf(data.maxHumidity, 4, 1, maxHumidityStr); 

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

static void drawGraphScreen(AppData& data) {
    
  float graphMin = floor(data.minTemp);
  float graphMax = ceil(data.maxTemp);
  int vRange = 63 - 14; // Vertikaler Bereich für den Graphen

  display.setFont(u8g2_font_ncenB08_tr);

    // Mindestrange von 1 Grad sicherstellen
    if (graphMin == graphMax) {
      graphMin = graphMin - 0.5f;
      graphMax = graphMax + 0.5f;
    }
    // Es werden zwei Datenpunkte gebraucht, um eine Linie zu zeichnen
    if (data.validSamples < 2) {
      display.drawStr(10, 30, "Collecting data...");
      return;
    }
    // Historie als Graph zeichnen, neueste Daten rechts
    for (int i = 1; i < data.validSamples; i++) {
      int index = (data.historyIndex - (i-1) + data.validSamples) % data.validSamples; // Ringpuffer-Index
      int prevIndex = (data.historyIndex - i + data.validSamples) % data.validSamples; // Vorheriger Index im Ringpuffer
      
      int x1 = 127 - i;
      int x2 = 127 - (i-1);
      
      int y1 = 63 - vRange*((data.tempHistory[prevIndex] - graphMin) / (graphMax - graphMin));
      int y2 = 63 - vRange*((data.tempHistory[index] - graphMin) / (graphMax - graphMin));

      display.drawLine(x1,y1,x2,y2);      
    }
    // Achsen zeichnen und Beschriftung hinzufügen
    display.drawLine(0, 14, 0, 63); // Y-Achse
    display.drawLine(0, 63, 127, 63); // X-Achse
    // Draw ticks on x-axis every 20 pixels
    for (int x = 10; x < 128; x += 10) {
      display.drawPixel(x, 62); // Ticks auf der X-Achse
    }
    // Draw ticks on y-axis every for half integer temperatures
    for (int i = 0; i <= floor((graphMax-graphMin)*2); i++) {
      float t = graphMin + (float)i / 2.0f;
      int y = 63 - 49*((t - graphMin) / (graphMax - graphMin));
      display.drawPixel(1, y); // Ticks auf der Y-Achse      
    }
    
    String header = "T: " + String(graphMin, 1) + " - " + String(graphMax, 1)  + "\xB0""C";
    display.drawStr(10, 12, header.c_str());
    String currentTempStr = String(data.temp, 1) + "\xB0""C";
    display.drawStr(100, 12, currentTempStr.c_str());
  }

  static void drawGraphScreenHum(AppData& data) {
    float graphMin = floor(data.minHumidity);
    float graphMax = ceil(data.maxHumidity);
    int vRange = 63 - 14; // Vertikaler Bereich für den Graphen

    display.setFont(u8g2_font_ncenB08_tr);

    // Mindestrange von 1 Grad sicherstellen
    if (graphMin == graphMax) {
      graphMin = graphMin - 0.5f;
      graphMax = graphMax + 0.5f;
    }
    // Es werden zwei Datenpunkte gebraucht, um eine Linie zu zeichnen
    if (data.validSamples < 2) {
      display.drawStr(10, 30, "Collecting data...");
      return;
    }
    // Historie als Graph zeichnen, neueste Daten rechts
    for (int i = 1; i < data.validSamples; i++) {
      int index = (data.historyIndex - (i-1) + data.validSamples) % data.validSamples; // Ringpuffer-Index
      int prevIndex = (data.historyIndex - i + data.validSamples) % data.validSamples; // Vorheriger Index im Ringpuffer
      
      int x1 = 127 - i;
      int x2 = 127 - (i-1);
      
      int y1 = 63 - vRange*((data.humidityHistory[prevIndex] - graphMin) / (graphMax - graphMin));
      int y2 = 63 - vRange*((data.humidityHistory[index] - graphMin) / (graphMax - graphMin));

      display.drawLine(x1,y1,x2,y2);      
    }
    // Achsen zeichnen und Beschriftung hinzufügen
    display.drawLine(0, 14, 0, 63); // Y-Achse
    display.drawLine(0, 63, 127, 63); // X-Achse
    // Draw ticks on x-axis every 20 pixels
    for (int x = 10; x < 128; x += 10) {
      display.drawPixel(x, 62); // Ticks auf der X-Achse
    }
    // Draw ticks on y-axis every for half integer temperatures
    for (int i = 0; i <= floor((graphMax-graphMin)); i++) {
      float t = graphMin + (float)i;
      int y = 63 - 49*((t - graphMin) / (graphMax - graphMin));
      display.drawPixel(1, y); // Ticks auf der Y-Achse      
    }
    
    String header = "H: " + String(graphMin, 1) + " - " + String(graphMax, 1)  + "%";
    display.drawStr(10, 12, header.c_str());
    String currentTempStr = String(data.humidity, 1) + "%";
    display.drawStr(100, 12, currentTempStr.c_str());
  }

void weather_show(AppData& data) {
  display.clearBuffer();

  // State Machine für den Bildschirm
  switch(currentScreen) {
    case SCREEN_MAIN:
        drawMainScreen(data);
        break;
    case SCREEN_MINMAX:
        drawMinMaxScreen(data);
        break;
    case SCREEN_GRAPH_TEMP:
        drawGraphScreen(data);
        break;
    case SCREEN_GRAPH_HUM:
        drawGraphScreenHum(data);
        break;
}
  display.sendBuffer();
  
}