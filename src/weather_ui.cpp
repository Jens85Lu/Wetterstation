// Anzeigelogik

#include "weather_ui.h"
#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>

extern U8G2_SH1106_128X64_NONAME_F_HW_I2C display;
static bool lastEdge = false;

void weather_show(float temp, float humidity) {
  static int x = 0; // movement in x direction
  static int y = 0; // movement in y direction
  char tempStr[16];
  char humidityStr[16];
  display.clearBuffer();
  display.setFont(u8g2_font_crox4hb_tf);
  dtostrf(temp, 4, 1, tempStr);
  dtostrf(humidity, 4, 1, humidityStr);
  display.drawStr(x, 20 + y, "T: ");
  display.drawStr(70,20 + y, tempStr);
  display.drawStr(110,20 + y, "\xB0""C"); // Gradzeichen
  display.drawStr(x, 40 + 1.1*y, "H: ");
  display.drawStr(70, 40 + 1.1*y, humidityStr);
  display.drawStr(110, 40 + 1.1*y, "%");

  display.sendBuffer();
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