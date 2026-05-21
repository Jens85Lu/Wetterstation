#include "button.h"
#include <Arduino.h>
#define BUTTON_PIN 3

static unsigned long lastPressTime = 0;
const unsigned long debounceDelay = 200; // Debounce-Zeit in Millisekunden

bool button_wasPressed() {
  static bool previous = HIGH; // Vorheriger Zustand des Buttons
  bool current = digitalRead(BUTTON_PIN);
  if (previous == HIGH && current == LOW) {
    unsigned long currentTime = millis();
    if (currentTime - lastPressTime > debounceDelay) {
      lastPressTime = currentTime;
      return true; // Button wurde gedrückt
    }
  }
  previous = current; // Aktualisiere den vorherigen Zustand
  return false; // Button wurde nicht gedrückt
}