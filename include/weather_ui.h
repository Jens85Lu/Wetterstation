#ifndef WEATHER_UI_H
#define WEATHER_UI_H
#include "app_state.h"
#include "display.h"

static const uint8_t sunBitmap[] PROGMEM = {
  0b00100100,
  0b00011000,
  0b10111101,
  0b01111110,
  0b11111111,
  0b01111110,
  0b10111101,
  0b00011000
};

  static const uint8_t cloudBitmap[] PROGMEM = {
  0b00000000,
  0b00111000,
  0b01111100,
  0b11111110,
  0b11111110,
  0b01111100,
  0b00111000,
  0b00000000
};

  static const uint8_t rainBitmap[] PROGMEM = {
  0b00111000,
  0b01111100,
  0b11111110,
  0b01111100,
  0b00101000,
  0b01000100,
  0b00101000,
  0b00000000
};

void weather_show(AppData& data);


#endif
