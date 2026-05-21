// Anzeigelogik

#include "weather_ui.h"
#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include "app_state.h"

extern U8G2_SH1106_128X64_NONAME_F_HW_I2C display;
static bool lastEdge = false; // For Screensaver animation
static int x = 0; // movement in x direction
static int y = 0; // movement in y direction

static float minTemp = 999.0f;
static float maxTemp = -999.0f;
static float minHumidity = 999.0f;
static float maxHumidity = -999.0f;

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
  display.drawStr(x, 20 + y, "T: ");
  display.drawStr(70,20 + y, tempStr);
  display.drawStr(110,20 + y, "\xB0""C"); // Gradzeichen
  display.drawStr(x, 40 + 1.1*y, "H: ");
  display.drawStr(70, 40 + 1.1*y, humidityStr);
  display.drawStr(110, 40 + 1.1*y, "%");
  
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
    display.setFont(u8g2_font_ncenB08_tr);

    display.drawStr(20, 30, "Graph Screen");
};

void weather_show(float temp, float humidity) {
  display.clearBuffer();
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