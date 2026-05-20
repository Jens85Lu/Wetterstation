// Anzeigelogik

#include "weather_ui.h"
#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>

extern U8G2_SH1106_128X64_NONAME_F_HW_I2C display;

void weather_show(float temp, float humidity) {
  char tempStr[16];
  char humidityStr[16];
  display.clearBuffer();
  display.setFont(u8g2_font_crox4hb_tf);
  dtostrf(temp, 4, 1, tempStr);
  dtostrf(humidity, 4, 1, humidityStr);
  display.drawStr(0, 20, "T: ");
  display.drawStr(70,20, tempStr);
  display.drawStr(110,20, "\xB0""C"); // Gradzeichen
  display.drawStr(0, 40, "H: ");
  display.drawStr(70, 40, humidityStr);
  display.drawStr(110, 40, "%");

  display.sendBuffer();
  
}