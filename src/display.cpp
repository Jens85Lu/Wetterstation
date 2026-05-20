// OLED initialisieren und low level Displayfunktionen

#include <Wire.h>
#include <U8g2lib.h>
#include "display.h"
#include <Arduino.h>

U8G2_SH1106_128X64_NONAME_F_HW_I2C display(U8G2_R0, 
    U8X8_PIN_NONE);

void display_init() {
  display.begin();
}

void display_showText(const char* text) {
  display.clearBuffer();
  display.setFont(u8g2_font_6x12_tf);
  display.drawStr(0, 30, text);
  display.sendBuffer();
}